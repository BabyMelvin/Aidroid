# 状态图

<image src="image/01-01.gif"/>

注：椭圆代表状态。箭头代表操作(改变状态)。单箭头代表同步方法调用，双箭头代表异步操作。

* `reset()`和`new`创建新的MediaPlayer对象，状态为`Idle`。`release()`调用后，状态为`End`.这两个中间为所有的声明周期。
	* `new`和`reset()`有稍微区别。在Idle状态下，调用下面方法都会有变成一个错误：`getCurrentPosition(), getDuration(), getVideoHeight(), getVideoWidth(), setAudioAttributes(AudioAttributes), setLooping(boolean), setVolume(float, float), pause(), start(), stop(), seekTo(long, int), prepare() or prepareAsync()`。如果`new`不会在`OnError()`中回调，状态不变。如果是`reset()`，将会回调`onError()`，并且进入`Error`状态。
	* 推荐MediaPlayer不再使用立即调用`release()`。会释放相关资源，资源可能包括单利资源如硬件加速组件，并且失败调用`release()`可能导致MediaPlayer回退到软件实现。进入`End`状态就不能返回任何状态。
	* 而且，`new`是`Idle`状态，但是重载`create`方法不是在`Idle`状态。`create`成功是在`Prepared`状态。
	
* 一般一些播放控制操作可能会失败，由于各种原因。如，**不支持的视音频格式**,**差质量隔行视音频**,**分辨率太高**，**流超时**等。因此，error报错和恢复在这种环境下很重要的，也有时，由于编程错误导致无效状态也有可能。需要注册`onErrorListener()`。
	* **注意**：一旦出错，MediaPlayer进入`Error`状态，即使应用没有注册错误监听。
	* 为了能够重复使用在Error状态的MediaPlayer对象,用`reset()`将MediaPlayer从`Error`状态变为`Idle`状态。
	* 很好的习惯注册错误监听，这样告诉内部处理。
	* 当编程错误`prepare()`,`prepareAsync`抛出`IllegalStateException`。或者重载`setDataSource`在一个无效的状态。
* 调用`setDataSource(FileDescriptor), or setDataSource(String), or setDataSource(Context, Uri), or setDataSource(FileDescriptor, long, long), or setDataSource(MediaDataSource)`播放器从`Idle`状态进入到`Initalized`状态。
	* 如果`setDataSource()`在其他状态调用，将会抛出`IllegalStateException`
	* 留意`setDataSoucrce`抛出来的`IllegalArgumentException`和`IOException`
* 播放前，播放器必须先进入`Prepared`状态。
	* 两种`prepare()`和`prepareAsync()`
	* 必须注意`Preparing`状态是瞬时状态，MediaPlayer对象的Preparing状态有副作用，没有进行定义。
	* 在其他状态调用，将会抛出`IllegalStateException`
	* 当在`Prepared`状态,这些属性能够通方法设置：如`音量`，`screenOnWhilePlaying`，`looping`。
* 开始播放`start()`调用，返回成功进入`Started`状态，用`isPlaying()`进行检测是否在`Started`状态。
	* 当在`Started`状态，内部播放引擎能够调用用户提供的`onBufferingUpdatae()`回调方法。这个能够使用户获得`buffering`状态当播放流媒体时候。

* 播放能后暂停`pause()`和停止`stop()`，并且当前的播放位置可以调整。`pause()`返回，进入`Paused`状态，注意，`Started`状态进入`Paused`状态是瞬时的。会花一点时间更新`isPlaying()`的状态，在流媒体时候可能花几秒的时间。
	* `start()`重新播放暂停的播放器对象，从暂停位置开始播放。
	* 重复调用`pause()`在`Paused`状态没有影响。

* `stop()`停止播放，导致一个`Started`，`Paused`,`Prepared`或`PlaybackCompleted`状态进入`Stopped`状态。
	* 一旦进入`Stopped`状态，播放器只有`prepare()`或`prepareAsync()`才能重新使用。
	* 在`Stopped`状态,重复调用`stop()`没有副作用

* 播放位置，可以通过`seekTo(long ,int)`:
	* 虽然异步`seekTo(long,int)`立刻放回，但是流媒体可能要花一会时间。当实际`seek`操作完成，播放器引擎回调`onSeekCompelteListener`
	* **注意**，`seekTo(long,int)`能够在其他状态下调用，如`Preared`,`Paused`和`PlaybackCompelted`状态。当在这些状态调用，如果位置有效将会展现该位置的帧。
	* 而且，可以通过`getCurrentPosition()`获得当前的位置，便于应用跟踪当前位置。
	
* 当播放到流末尾，播放完成。
	* 如果`setLooping(boolean)`为true，播放器仍然保持`Started`状态。
	* 如果为false，`onCompeltetion()`被回调。进入`PlaybackCompelted`状态。
	* 当播放完成，可以进入`Started`，从头开始播放。