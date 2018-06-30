# MediaPlayer 使用

* 本地资源
* 网络URIs
* 流

最简单播放本地`raw resource`。

## 1.raw 资源
```java
MediaPlayer mediaPlayer=MediaPlayer.create(context,R.raw.sound_file_1);

// no need to call prepare(); create() does that for you
mediaPlayer.start();
```

## 2.URI 资源

```java
Uri myUri = ....; // initialize Uri here
MediaPlayer mediaPlayer = new MediaPlayer();
mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
mediaPlayer.setDataSource(getApplicationContext(), myUri);
mediaPlayer.prepare();
mediaPlayer.start();
```

## 3.播放HTTP流

```java
String url = "http://........"; // your URL here
MediaPlayer mediaPlayer = new MediaPlayer();
mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
mediaPlayer.setDataSource(url);
mediaPlayer.prepare(); // might take long! (for buffering, etc)
mediaPlayer.start();
```

**小心**：`setDataSource()`不存在将会获得 `IllegalArgumentException` 和`IOException`

# 异步准备

`prepare()`**never call it from your application's UI thread**.应该使用`prepareAsync()`方法，`onPrepared()`方法回调。 

# 状态图
**Always keep the state diagram in mind when writing code that interacts with a MediaPlayer object**, because calling its methods from the wrong state is a common cause of bugs.

# 释放MediaPlayer
`MediaPlayer`消耗系统资源，不再需要的时候释放。

```java
mediaPlayer.release();
mediaPlayer=null;
```

当Activity停止了，而没有去释放，屏幕旋转后又创建一个新的，导致消耗很多系统资源。

有些配置可能会运行时发生变化(键盘可用性及语言，屏幕方向)。这是Android重启正在运行的Activity(`onDestory()`和`onCreate()`)，重启目的来适配新配置。

**妥善处理重启行为**：销毁Activity之前调用`onSaveInstanceState()`，之后在`onCreate()`或`onRestoreInstanceState()`期间恢复Activity状态。

重启恢复成本较高，解决方案:

* 1.在配置变更期间保留对象：运行配置变更时重启，但将状态对象传递给Acitivity新实例。
* 2.自行处理配置变更：阻止系统某些配置重启Activity，但在配置确实发生变化接收回调，手动更新`Activity`.

### 在配置变更期间保留对象

依靠系统通过`onSaveInstanceState() `回调为您保存的 Bundle，可能无法完全恢复 Activity 状态，因为它并非设计用于携带大型对象（例如位图），而且其中的数据必须先序列化，再进行反序列化，这可能会消耗大量内存并使得配置变更速度缓慢。 在这种情况下，如果 Activity 因配置变更而重启，则可通过保留 Fragment 来减轻重新初始化 Activity 的负担。此片段可能包含对您要保留的有状态对象的引用。

当 Android 系统因配置变更而关闭 Activity 时，不会销毁您已标记为要保留的 Activity 的片段。 您可以将此类片段添加到 Activity 以保留有状态的对象。

要在运行时配置变更期间将有状态的对象保留在片段中，请执行以下操作：

* 1.扩展 Fragment 类并声明对有状态对象的引用。
* 在创建片段后调用 `setRetainInstance(boolean)`.
* 将片段添加到 Activity。
* 重启 Activity 后，使用 `FragmentManager` 检索片段。

```java
public class RetainedFragment extends Fragment {

    // data object we want to retain
    private MyDataObject data;

    // this method is only called once for this fragment
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // retain this fragment
        setRetainInstance(true);
    }

    public void setData(MyDataObject data) {
        this.data = data;
    }

    public MyDataObject getData() {
        return data;
    }
}
```
**注意**：尽管您可以存储任何对象，但是切勿传递与 Activity 绑定的对象，例如，Drawable、Adapter、View 或其他任何与 Context 关联的对象。否则，它将泄漏原始 Activity 实例的所有视图和资源。 （泄漏资源意味着应用将继续持有这些资源，但是无法对其进行垃圾回收，因此可能会丢失大量内存。）

然后，使用 `FragmentManager` 将片段添加到 Activity。在运行时配置变更期间再次启动 Activity 时，您可以获得片段中的数据对象。 例如，按如下方式定义 Activity：

```java
public class MyActivity extends Activity {

    private RetainedFragment dataFragment;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        // find the retained fragment on activity restarts
        FragmentManager fm = getFragmentManager();
        dataFragment = (DataFragment) fm.findFragmentByTag(“data”);

        // create the fragment and data the first time
        if (dataFragment == null) {
            // add the fragment
            dataFragment = new DataFragment();
            fm.beginTransaction().add(dataFragment, “data”).commit();
            // load the data from the web
            dataFragment.setData(loadMyData());
        }

        // the data is available in dataFragment.getData()
        ...
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // store the data in the fragment
        dataFragment.setData(collectMyLoadedData());
    }
}
```
在此示例中，`onCreate()` 添加了一个片段或恢复了对它的引用。此外，`onCreate()` 还将有状态的对象存储在片段实例内部。`onDestroy()`对所保留的片段实例内的有状态对象进行更新。

## 2.自行处理配置变更
如果应用在特定配置变更期间无需更新资源，并且因性能限制您需要尽量避免重启，则可声明 Activity 将自行处理配置变更，这样可以阻止系统重启 Activity。

**注**：自行处理配置变更可能导致备用资源的使用更为困难，因为系统不会为您自动应用这些资源。 只能在您必须避免 Activity 因配置变更而重启这一万般无奈的情况下，才考虑采用自行处理配置变更这种方法，而且对于大多数应用并不建议使用此方法。

要声明由 Activity 处理配置变更，请在清单文件中编辑相应的 `<activity>` 元素，以包含 `android:configChanges` 属性以及代表要处理的配置的值。`android:configChanges` 属性的文档中列出了该属性的可能值（最常用的值包括 "orientation" 和 "keyboardHidden"，分别用于避免因屏幕方向和可用键盘改变而导致重启）。您可以在该属性中声明多个配置值，方法是用管道 `| `字符分隔这些配置值。

```html
<activity android:name=".MyActivity"
          android:configChanges="orientation|keyboardHidden"
          android:label="@string/app_name">
```
现在，当其中一个配置发生变化时，MyActivity 不会重启。相反，MyActivity 会收到对 `onConfigurationChanged()` 的调用。向此方法传递 Configuration 对象指定新设备配置。您可以通过读取 Configuration 中的字段，确定新配置，然后通过更新界面中使用的资源进行适当的更改。调用此方法时，Activity 的 Resources 对象会相应地进行更新，以根据新配置返回资源，这样，您就能够在系统不重启 Activity 的情况下轻松重置 UI 的元素。

**注意**：注意：从 `Android 3.2`（API 级别 13）开始，当设备在纵向和横向之间切换时，“屏幕尺寸”也会发生变化。因此，在开发针对 API 级别 13 或更高版本（正如 `minSdkVersion` 和 `targetSdkVersion` 属性中所声明）的应用时，若要避免由于设备方向改变而导致运行时重启，则除了 "orientation" 值以外，您还必须添加 "screenSize" 值。 也就是说，您必须声明 android:configChanges="orientation|screenSize"。但是，如果您的应用面向 API 级别 12 或更低版本，则 Activity 始终会自行处理此配置变更（即便是在 Android 3.2 或更高版本的设备上运行，此配置变更也不会重启 Activity）

以下 `onConfigurationChanged()` 实现检查当前设备方向：

```java
@Override
public void onConfigurationChanged(Configuration newConfig) {
    super.onConfigurationChanged(newConfig);

    // Checks the orientation of the screen
    if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
        Toast.makeText(this, "landscape", Toast.LENGTH_SHORT).show();
    } else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT){
        Toast.makeText(this, "portrait", Toast.LENGTH_SHORT).show();
    }
}
```

`Configuration `对象代表所有当前配置，而不仅仅是已经变更的配置。大多数时候，您并不在意配置具体发生了哪些变更，而且您可以轻松地重新分配所有资源，为您正在处理的配置提供备用资源。 例如，由于 `Resources` 对象现已更新，因此您可以通过 `setImageResource() `重置任何 `ImageView`，并且使用适合于新配置的资源（如提供资源中所述）。

**请注意**，Configuration 字段中的值是与 Configuration 类中的特定常量匹配的整型数。有关要对每个字段使用哪些常量的文档，请参阅 Configuration 参考文档中的相应字段。

**请谨记**：在声明由 Activity 处理配置变更时，您有责任重置要为其提供备用资源的所有元素。 如果您声明由 Activity 处理方向变更，而且有些图像应该在横向和纵向之间切换，则必须在 `onConfigurationChanged()` 期间将每个资源重新分配给每个元素。

如果无需基于这些配置变更更新应用，则可不用实现 `onConfigurationChanged()`。在这种情况下，仍将使用在配置变更之前用到的所有资源，只是您无需重启 Activity。 但是，应用应该始终能够在保持之前状态完好的情况下关闭和重启，因此您不得试图通过此方法来逃避在正常 Activity 生命周期期间保持您的应用状态。 这不仅仅是因为还存在其他一些无法禁止重启应用的配置变更，还因为有些事件必须由您处理，例如用户离开应用，而在用户返回应用之前该应用已被销毁。

# 服务中MediaPlayer使用

如果后台应用（没有界面的），必须使用Service控制后天MediaPlayer实例。需要`MediaBrowserServiceCompat`服务与在另一个Activity中的`MediaBrowserCompat`进行交互。<a herf="https://developer.android.com/guide/topics/media-apps/audio-app/building-an-audio-app">Audio app overview</a>

## 1.异步运行

```java
public class MyService extends Service implements MediaPlayer.OnPreparedListener {
    private static final String ACTION_PLAY = "com.example.action.PLAY";
    MediaPlayer mMediaPlayer = null;

    public int onStartCommand(Intent intent, int flags, int startId) {
        ...
        if (intent.getAction().equals(ACTION_PLAY)) {
            mMediaPlayer = ... // initialize it here
            mMediaPlayer.setOnPreparedListener(this);
            mMediaPlayer.prepareAsync(); // prepare async to not block main thread
        }
    }

    /** Called when MediaPlayer is ready */
    public void onPrepared(MediaPlayer player) {
        player.start();
    }
}
```

## 2.处理异步错误
service不能处理重量计算，如果有，用异步处理。

```java
public class MyService extends Service implements MediaPlayer.OnErrorListener {
    MediaPlayer mMediaPlayer;

    public void initMediaPlayer() {
        // ...initialize the MediaPlayer here...

        mMediaPlayer.setOnErrorListener(this);
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        // ... react appropriately ...
        // The MediaPlayer has moved to the Error state, must be reset!
    }
}
```
记住：当MediaPlayer进入到`Error State`，再次使用，要调用`reset`.

## 3.使用唤醒锁
当睡眠状态，Android 系统试着关闭不必要的设备包括CPU和WIFI硬件。然后，你的服务播放音乐，阻止设备关闭你的播放。使用`wake lock`。这是个信号，通知系统你的应用将会保持即使设备闲置状态。

确保当播放时，CPU继续运行，在初始化时候，调用`setWakeMode()`。播放器在播放时候会保持锁，暂停或停止时，释放锁。

```java
mMediaPlayer=new MediaPlayer();
// ... other initialization here ...
mMediaPlayer.setWakeMode(getApplicationContext(), PowerManager.PARTIAL_WAKE_LOCK);
```
然后，这个锁仅仅保证CPU保持唤醒，如果使用流媒体通过网络需要使用WIFI，可能需要`WifiLock`：

```java
WifiLock wifiLock = ((WifiManager) getSystemService(Context.WIFI_SERVICE))
    .createWifiLock(WifiManager.WIFI_MODE_FULL, "mylock");

wifiLock.acquire();
```
当暂停或停止媒体，应该释放锁：

```java
wifiLock.release();
```

## 4.清理

```java
public class MyService extends Service {
   MediaPlayer mMediaPlayer;
   // ...

   @Override
   public void onDestroy() {
       if (mMediaPlayer != null) mMediaPlayer.release();
   }
}
```
除了关机释放，应该寻找其他机会释放MediaPlayer:

* 如果确定不能播放更多媒体(失去音乐焦点后)，很确定的去释放`MediaPlayer`，之后再进行创建。
* 如果要暂停很短的时间，应当保持住MediaPlayer,避免创建和准备的时间消耗。

# DRM数字专利管理
Android8.0 支持DRM-protected资料。