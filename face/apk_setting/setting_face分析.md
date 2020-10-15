android 10将Face和Fingerprint归为Biometric类。

主Activity为：

> E:\Settings\src\com\android\settings\biometrics\BiometricEnrollActivity.java

```java
@Override
public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    final PackageManager pm = getApplicationContext().getPackageManager();
    Intent intent = null;

    // This logic may have to be modified on devices with multiple biometrics.
    if (pm.hasSystemFeature(PackageManager.FEATURE_FINGERPRINT)) {
        // ChooseLockGeneric can request to start fingerprint enroll bypassing the intro screen.
        if (getIntent().getBooleanExtra(EXTRA_SKIP_INTRO, false)
                && this instanceof InternalActivity) {
            intent = getFingerprintFindSensorIntent();
        } else {
            intent = getFingerprintIntroIntent();
        }
    } else if (pm.hasSystemFeature(PackageManager.FEATURE_FACE)) {
        intent = getFaceIntroIntent();
    }

    if (intent != null) {
        intent.setFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT);

        if (this instanceof InternalActivity) {
            // Propagate challenge and user Id from ChooseLockGeneric.
            final byte[] token = getIntent()
                    .getByteArrayExtra(ChooseLockSettingsHelper.EXTRA_KEY_CHALLENGE_TOKEN);
            final int userId = getIntent()
                    .getIntExtra(Intent.EXTRA_USER_ID, UserHandle.USER_NULL);

            intent.putExtra(ChooseLockSettingsHelper.EXTRA_KEY_CHALLENGE_TOKEN, token);
            intent.putExtra(Intent.EXTRA_USER_ID, userId);
        }

        startActivity(intent);
    }
    finish();
}
```

* 1.如果都支持fingerprint和face，需要修改下逻辑.是否要介绍的逻辑，ChooseLockGeneric可以设置跳过fingerprint的介绍。
* 2.分析下face过程.打开介绍activity并传入带有ChooseLockSettingsHelper.EXTRA_KEY_CHALLENGE_TOKEN,Intent.EXTRA_USER_ID参数和flags为Intent.FLAG_ACTIVITY_FORWARD_RESULT
	* Intent.FLAG_ACTIVITY_FORWARD_RESULT作用:如果设置，并且这个Intent用于从一个存在的Activity启动一个新的Activity，那么，这个作为答复目标的Activity将会传到这个新的Activity中。这种方式下，新的Activity可以调用setResult(int)，并且这个结果值将发送给那个作为答复目标的Activity。
* 3.然后结束当前的activity。

intent的内容如下:

```java
private Intent getFaceIntroIntent() {
    Intent intent = new Intent(this, FaceEnrollIntroduction.class);
    WizardManagerHelper.copyWizardManagerExtras(getIntent(), intent);
    return intent;
}
```

* 启动FaceEnrollIntroduction
* 启动FaceEnrollIntroduction extend BiometricEnrollIntroduction，BiometricEnrollIntroduction是抽象类，主要逻辑还是在这个基类实现的。abstract方法是在启动FaceEnrollIntroduction中实现.

最终实现的录入的界面：FaceEnrollEnrolling

# 1.录入主界面

```xml
<com.google.android.setupdesign.GlifLayout
    xmlns:android="http://schemas.android.com/apk/res/android"
    android:id="@+id/setup_wizard_layout"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    style="?attr/face_layout_theme">

    <LinearLayout
        style="@style/SudContentFrame"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        android:orientation="vertical"
        android:clipToPadding="false"
        android:clipChildren="false">

        <LinearLayout
            android:layout_width="match_parent"
            android:layout_height="0dp"
            android:layout_weight="1"
            android:gravity="center"
            android:orientation="vertical">

			<!--自定义布局-->
            <com.android.settings.biometrics.face.FaceSquareFrameLayout
                android:layout_width="match_parent"
                android:layout_height="match_parent"
                android:layout_weight="1">
				<!-- 显示的内容，用来预览-->
                <com.android.settings.biometrics.face.FaceSquareTextureView
                    android:id="@+id/texture_view"
                    android:layout_width="wrap_content"
                    android:layout_height="wrap_content"
                    android:contentDescription="@null" />
				<!-- 圆形空出来 -->
                <ImageView
                    android:id="@+id/circle_view"
                    android:layout_width="match_parent"
                    android:layout_height="match_parent" />

            </com.android.settings.biometrics.face.FaceSquareFrameLayout>
			
			<!-- 预览下面，显示可能的错误信息 -->
            <TextView
                style="@style/TextAppearance.ErrorText"
                android:id="@+id/error_text"
                android:layout_width="wrap_content"
                android:layout_height="wrap_content"
                android:layout_gravity="center_horizontal|bottom"
                android:accessibilityLiveRegion="polite"
                android:gravity="center"
                android:visibility="invisible"/>

        </LinearLayout>

    </LinearLayout>

</com.google.android.setupdesign.GlifLayout>
```

## 1.1 FaceSquareFrameLayout

```java
@Override
protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    // Don't call super, manually set their size below
    int size = MeasureSpec.getSize(widthMeasureSpec);

    // Set this frame layout to be a square
	// TODO 设置为正方形
    setMeasuredDimension(size, size);

    // Set the children to be the same size (square) as well
    final int numChildren = getChildCount();
	// 子类型也要进行设置为方形
    for (int i = 0; i < numChildren; i++) {
        int spec = MeasureSpec.makeMeasureSpec(size, MeasureSpec.EXACTLY);
        this.getChildAt(i).measure(spec, spec);
    }
}
```

* 主要主界面，显示的view，都设置为正方形显示

## 1.2 FaceSquareTextureView

```java
// 为了显示摄像头内容的预览(这个不是录入的真正数据)
public class FaceSquareTextureView extends TextureView {
	 @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (width < height) {
            setMeasuredDimension(width, width);
        } else {
            setMeasuredDimension(height, height);
        }
    }
}
```

* 也设置为方形

## 1.3 imageView

主要看下：

* match_parent,match_parent
* 为了添加预览的界面，透明，露出圆形

## 1.4 TextView

主要是为了显示错误信息

刚开始`android:visibility="invisible"`


# 2.看下FaceEnrollEnrolling

## 2.1 onCreate

```java
protected void onCreate(Bundle savedInstanceState) {
	// 父类获得mToken，mFromSettingSummary,mUserId
	super.onCreate(savedInstanceState);
	// 添加录入主界面显示
    setContentView(R.layout.face_enroll_enrolling);
	// 显示文本内容：Center your face in the circle
	setHeaderText(R.string.security_settings_face_enroll_repeat_title);
    mErrorText = findViewById(R.id.error_text);
	// 下面函数
	startEnrollment();
}

@Override
public void startEnrollment() {
	// TODO 父类,mSidecar(Fragment)，mSidercar.setListener(this)
    super.startEnrollment();
	
	// TODO 用来预览的界面,这里初始化了textureview 和 imageview 重点分析
    mPreviewFragment = (FaceEnrollPreviewFragment) getSupportFragmentManager()
            .findFragmentByTag(TAG_FACE_PREVIEW);
    if (mPreviewFragment == null) {
        mPreviewFragment = new FaceEnrollPreviewFragment();
        getSupportFragmentManager().beginTransaction().add(mPreviewFragment, TAG_FACE_PREVIEW)
                .commitAllowingStateLoss();
    }

	// 这里监听，enrolled完成，则结束主activity
    mPreviewFragment.setListener(mListener);
}
```

### 2.1.1 mSidecar

> mSidecar(Fragment)

```java
//E:\melvin\Settings\src\com\android\settings\biometrics\BiometricEnrollSidecar.java

@Override
public void onStart() {
    super.onStart();
    if (!mEnrolling) {
        startEnrollment();
    }
}
protected void startEnrollment() {
    mHandler.removeCallbacks(mTimeoutRunnable);
    mEnrollmentSteps = -1;
    mEnrollmentCancel = new CancellationSignal();
    mEnrolling = true;
}
```

> `mSidercar.setListener(this)`

this：`public abstract class BiometricsEnrollEnrolling extends BiometricEnrollBase implements BiometricEnrollSidecar.Listener`

抽象类，实现接口内容为

```java
public interface Listener {
    void onEnrollmentHelp(int helpMsgId, CharSequence helpString);
    void onEnrollmentError(int errMsgId, CharSequence errString);
    void onEnrollmentProgressChange(int steps, int remaining);
}
```

> **注意**：这里抽象类BiometricsEnrollEnrolling并没有去实现接口，那么则由子类去实现了

查看，发现FaceEnrollEnrolling确实实现了内容：

```java
 @Override
public void onEnrollmentHelp(int helpMsgId, CharSequence helpString) {
    if (!TextUtils.isEmpty(helpString)) {
        showError(helpString);
    }
    mPreviewFragment.onEnrollmentHelp(helpMsgId, helpString);
}

@Override
public void onEnrollmentError(int errMsgId, CharSequence errString) {
    int msgId;
    switch (errMsgId) {
        case FaceManager.FACE_ERROR_TIMEOUT:
            msgId = R.string.security_settings_face_enroll_error_timeout_dialog_message;
            break;
        default:
            msgId = R.string.security_settings_face_enroll_error_generic_dialog_message;
            break;
    }
    mPreviewFragment.onEnrollmentError(errMsgId, errString);
    showErrorDialog(getText(msgId), errMsgId);
}

@Override
public void onEnrollmentProgressChange(int steps, int remaining) {
    if (DEBUG) {
        Log.v(TAG, "Steps: " + steps + " Remaining: " + remaining);
    }
    mPreviewFragment.onEnrollmentProgressChange(steps, remaining);

    // TODO: Update the actual animation
    showError("Steps: " + steps + " Remaining: " + remaining);

    // TODO: Have this match any animations that UX comes up with
    if (remaining == 0) {
        launchFinish(mToken);
    }
}
```

所以mSidercar中回掉的Listener内容真正指向了FaceEnrollEnrolling的实现

## 2.1.2 mPreviewFragment

> new FaceEnrollPreviewFragment()

```java
 @Override
public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    mTextureView = getActivity().findViewById(R.id.texture_view);
    mCircleView = getActivity().findViewById(R.id.circle_view);

    // Must disable hardware acceleration for this view, otherwise transparency breaks
    mCircleView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);

    mAnimationDrawable = new FaceEnrollAnimationDrawable(getContext(), mAnimationListener);
    mCircleView.setImageDrawable(mAnimationDrawable);

    mCameraManager = (CameraManager) getContext().getSystemService(Context.CAMERA_SERVICE);
}
```

* 1.对主界面进行操作
* 2.circleView进行设置类型，动画
* 3.获得cameraManager

circleView使用了FaceEnrollAnimationDrawable自定义Drawable

```java
public class FaceEnrollAnimationDrawable extends Drawable
        implements BiometricEnrollSidecar.Listener {
	private static final int BORDER_BOUNDS = 20;
	
	private final Context mContext;
	private final ParticleCollection.Listener mListener;
	private Rect mBounds;
	private final Paint mSquarePaint;
	private final Paint mCircleCutoutPaint;
	private ParticleCollection mParticleCollection;
	private TimeAnimator mTimeAnimator;
	
	private final ParticleCollection.Listener mAnimationListener
            = new ParticleCollection.Listener() {
        @Override
        public void onEnrolled() {
            if (mTimeAnimator != null && mTimeAnimator.isStarted()) {
                mTimeAnimator.end();
                mListener.onEnrolled();
            }
        }
    };

	public FaceEnrollAnimationDrawable(Context context, ParticleCollection.Listener listener) {
		mContext = context;
		mListener = listener;
	
		mSquarePaint = new Paint();
		mSquarePaint.setColor(Color.WHITE);
		mSquarePaint.setAntiAlisa(true);
	
		mCircleCutoutPaint = new Paint();
		mCircleCutoutPaint.setColor(Color.TRANSPARENT);
		mCircleCutoutPaint.setXfermode(new PorterDuffXfermode(PorterDuff.mode.CLEAR);
		mCircleCutoutPaint.setAntiAlias(true);
	}

	@Override
	protected void onBoundsChange(Rect bounds) {
		mBounds = bounds;
        mParticleCollection =
                new ParticleCollection(mContext, mAnimationListener, bounds, BORDER_BOUNDS);
		if (mTimeAnimator == null) {
            mTimeAnimator = new TimeAnimator();
            mTimeAnimator.setTimeListener((animation, totalTimeMs, deltaTimeMs) -> {
                mParticleCollection.update(totalTimeMs, deltaTimeMs);
                FaceEnrollAnimationDrawable.this.invalidateSelf();
            });
            mTimeAnimator.start();
        }
	}
	
	@Override
	public void draw(Canvas canvas) {
		if (mBounds == null) {
			return;
		}
		
		if (mBounds == null) {
            return;
        }
        canvas.save();

        // Draw a rectangle covering the whole view
        canvas.drawRect(0, 0, mBounds.width(), mBounds.height(), mSquarePaint);

        // Clear a circle in the middle for the camera preview
        canvas.drawCircle(mBounds.exactCenterX(), mBounds.exactCenterY(),
                mBounds.height() / 2 - BORDER_BOUNDS, mCircleCutoutPaint);

        // Draw the animation
        mParticleCollection.draw(canvas);

        canvas.restore();
	}
}
```

需要看下ParticleCollection

这个类用来创建，维护和更新动画，作为细节内容处理类

`public class ParticleCollection implements BiometricEnrollSidecar.Listener`

textureView进行分析:

```java
@Override
public void onResume() {
    super.onResume();
	mTextureView.setSurfaceTextureListener(mSurfaceTextureListener);
}
```

* mSurfaceTextureListener主要是为了打开辅助摄像头，preview采集的内容

```java
private void openCamera(int width, int height) {
    try {
		// 主要找到前置摄像头，获得stream配置，选择合适的显示
        setUpCameraOutputs();

		// 打开摄像头
        mCameraManager.openCamera(mCameraId, mCameraStateCallback, mHandler);
		// 设置好texture view的宽高
        configureTransform(width, height);
    } catch (CameraAccessException e) {
        Log.e(TAG, "Unable to open camera", e);
    }
}
```

主要看下mCameraStateCallback和mHandler

```java
// mCameraStateCallback
private final CameraDevice.StateCallback mCameraStateCallback =
            new CameraDevice.StateCallback() {

@Override
public void onOpened(CameraDevice cameraDevice) {
    mCameraDevice = cameraDevice;
    try {
        // Configure the size of default buffer
        SurfaceTexture texture = mTextureView.getSurfaceTexture();
		// previewSize在设置camera参数时候选定了
        texture.setDefaultBufferSize(mPreviewSize.getWidth(), mPreviewSize.getHeight());

        // This is the output Surface we need to start preview
        Surface surface = new Surface(texture);

        // Set up a CaptureRequest.Builder with the output Surface
        mPreviewRequestBuilder =
                mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        mPreviewRequestBuilder.addTarget(surface);

        // Create a CameraCaptureSession for camera preview
        mCameraDevice.createCaptureSession(Arrays.asList(surface),
            new CameraCaptureSession.StateCallback() {

                @Override
                public void onConfigured(CameraCaptureSession cameraCaptureSession) {
                    // The camera is already closed
                    if (null == mCameraDevice) {
                        return;
                    }
                    // When the session is ready, we start displaying the preview.
                    mCaptureSession = cameraCaptureSession;
                    try {
                        // Auto focus should be continuous for camera preview.
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                                CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);

                        // Finally, we start displaying the camera preview.
                        mPreviewRequest = mPreviewRequestBuilder.build();
                        mCaptureSession.setRepeatingRequest(mPreviewRequest,
                                null /* listener */, mHandler);
                    } catch (CameraAccessException e) {
                        Log.e(TAG, "Unable to access camera", e);
                    }
                }

                @Override
                public void onConfigureFailed(CameraCaptureSession cameraCaptureSession) {
                    Log.e(TAG, "Unable to configure camera");
                }
            }, null /* handler */);
    } catch (CameraAccessException e) {
        e.printStackTrace();
    }
}
```

可以看到apk层,camera操作流程:

* 1.设置camera参数
	* 遍历获得camera id,`String cameraId = mCameraManager.getCameraIdList()`
	* `CameraCharacteristics characteristics = mCameraManager.getCameraCharacteristics(cameraId);`
	* 找到前置摄像头:`Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);`
	* 获得流的配置：`StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);`,找到最合适，找不到就用`map[0]`
* 2.mCameraManager.openCamera打开摄像头, 打开时候回调进行处理onOpened
	* `mCameraDevice = cameraDevice;`
	* 通过texture来获得Surface
	* CaptureRequest.Builder
		* `mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);`
		* `mPreviewRequestBuilder.addTarget(surface);`
	* 创建一个CameraCaptureSession来camera预览
		* `mCameraDevice.createCaptureSession`，参数为surface和CameraCaptureSession.StateCallback
		* onConfigured:// When the session is ready, we start displaying the preview.
			* ` mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,                           CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);`
			*  // Finally, we start displaying the camera preview.
			*   `mPreviewRequest = mPreviewRequestBuilder.build();`
			*   `mCaptureSession.setRepeatingRequest(mPreviewRequest,null /* listener */, mHandler);`
			*   流关闭的方法为: `mCaptureSession.close();`

> mPreviewFragment.setListener(mListener);

主要用来onEnrolled关闭当前主界面

# 3.分析Listener

`com/android/settings/biometrics/face/FaceEnrollEnrolling.java`
这个状态主要是`BiometricEnrollSidecar`逻辑进行更新的，该类是抽象类，要找到真正的实现FaceEnrollSidecar，

其中在startEnrollment中


```java
public void startEnrollment() {
    mSidecar = (BiometricEnrollSidecar) getSupportFragmentManager()
            .findFragmentByTag(TAG_SIDECAR);
    if (mSidecar == null) {
		// 由子类去实现，在FaceEnrollEnrolling中实现
        mSidecar = getSidecar();
        getSupportFragmentManager().beginTransaction().add(mSidecar, TAG_SIDECAR)
                .commitAllowingStateLoss();
    }
    mSidecar.setListener(this);
}

// com/android/settings/biometrics/face/FaceEnrollEnrolling.java
@Override
protected BiometricEnrollSidecar getSidecar() {
    final int[] disabledFeatures = new int[mDisabledFeatures.size()];
    for (int i = 0; i < mDisabledFeatures.size(); i++) {
        disabledFeatures[i] = mDisabledFeatures.get(i);
    }

    return new FaceEnrollSidecar(disabledFeatures);
}
```

FaceEnrollSidecar类：

```java
public class FaceEnrollSidecar extends BiometricEnrollSidecar {

    private final int[] mDisabledFeatures;

    private FaceManager mFaceManager;

    public FaceEnrollSidecar(int[] disabledFeatures) {
        mDisabledFeatures = Arrays.copyOf(disabledFeatures, disabledFeatures.length);
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        mFaceManager = Utils.getFaceManagerOrNull(activity);
    }

    @Override
    public void startEnrollment() {
        super.startEnrollment();
        if (mUserId != UserHandle.USER_NULL) {
            mFaceManager.setActiveUser(mUserId);
        }
		// 这里真正和FaceManager进行交互了,注意注册了mEnrollmentCallback
        mFaceManager.enroll(mToken, mEnrollmentCancel,
                mEnrollmentCallback, mDisabledFeatures);
    }

    private FaceManager.EnrollmentCallback mEnrollmentCallback
            = new FaceManager.EnrollmentCallback() {

        @Override
        public void onEnrollmentProgress(int remaining) {
			// 传递给了BiometricEnrollSidecar，最终给了FaceEnrollEnrolling
            FaceEnrollSidecar.super.onEnrollmentProgress(remaining);
        }

        @Override
        public void onEnrollmentHelp(int helpMsgId, CharSequence helpString) {
            FaceEnrollSidecar.super.onEnrollmentHelp(helpMsgId, helpString);
        }

        @Override
        public void onEnrollmentError(int errMsgId, CharSequence errString) {
            FaceEnrollSidecar.super.onEnrollmentError(errMsgId, errString);
        }
    };

    @Override
    public int getMetricsCategory() {
        return SettingsEnums.FACE_ENROLL_SIDECAR;
    }
}
```

# 总结

Face录入操作，apk层主要进行了

* 1.`FaceManager mFaceManager = (FaceManager) context.getSystemService(Context.FACE_SERVICE);`
* 2.`mFaceManager.setActiveUser(mUserId);`
* 3.`mFaceManager.enroll(mToken, mEnrollmentCancel,mEnrollmentCallback, mDisabledFeatures);`
	* mEnrollmentCancel可用来取消
	* mEnrollmentCallback主要用来报错和remaining的回调
	* mDisabledFeatures：一些feature控制
