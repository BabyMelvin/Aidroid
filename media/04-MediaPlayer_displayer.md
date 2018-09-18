# 1.Surface
MediaPlayer使用`setDisplay(SurfaceHolder sh)`来显示媒体的视频部分。

* 当没有设置surfaceHolder或Surface会导致黑屏。
* 当设置为null也会播放只有声音，黑屏。

```java
public void setDisplay(SurfaceHolder sh){
	mSurfaceHolder=sh;
	Surface surface;
	surface=sh.getSurface();
	if(surface!=null&&surface.osd_video_flag)
		needosdvideo=surface.osd_video_flag;
	_setVideoSurface(surface);
	updateSurfaceScreenOn();
}
```
调用jni

```cpp
static void setVideoSurface(JNIEnv*env,jobject thiz,jobject jsurface,jboolean mediaPlayerMustBeAlive){
	sp<MediaPlayer> mp=getMediaPlayer(env,thiz);
	if(mp==NULL){
		if(mediaPlayerMustBeAlive)
			jniThrowException(env,"java/lang/IllegalStateException",NULL);
		return;
	}
	//将老的surface，decStrong
	decVideoSurfaceRef(env,thiz);
	sp<IGraphicBufferProducer> new_st;
	if(jsurface){
		sp<Surface> surface(android_view_Surface_getSurface(env,jsurface));
	}
	if(surface!=NULL){
		new_st=surface->getIGraphicBufferProducer();
		if(new_st==NULL){
			jniThrowException(env,"java/lang/IllegalArgumentException","The surface doest not have a binding SurfaceTexture");
			return;
		}
		new_st->incStrong((void*)decVideoSurfaceRef);
	}else{
		jniThrowException(env,"java/lang/IllegalArgumentException",""The surface has been released"");
		return;
	}
	env->SetIntField(thiz,fields.surface_texture,(int)new_st.get());
	mp->setVideoSurfaceTexture(new_st);
}
```

其中比价重要的是`android_view_Surface_getSurface(JNIEnv* env,jobject surfaceObj)`.

```cpp
sp<Surface> android_view_Surface_getSurface(JNIEnv* env,jobject surfaceObj){
	sp<Surface> sur;
	jobject lock=env->GetObjectField(surfaceObj,gSurfaceClassInfo.mLock);
	//添加锁lock
	if(env->MonitorEnter(lock)==JNI_OK){
		//获取保存在java的nativeSurface对象。
		sur=reinterpret_cast<Surface*>(evn->GetIntField(surfaceObj,gSurfaceClassInfo.mNativeObject));
		env->MontorExit(lock);
	}
	return sur;
}
```

通过MediaPlayer.cpp调到MediaPlayerService.cpp中的`setVideoSurfaceTexture`.

```cpp
status_t MediaPlayerService::Client::setVideoSurfaceTexture(const sp<IGraphicBufferProducer>& bufferProducer){
	sp<MediaPlayerBase> p=getPlayer();
	sp<IBinder> binder(bufferProducer==NULL?NULL:bufferProducer->asBinder());
	if(mConnectedWindowBinder==binder)
		return OK;
	sp<ANativeWindow> anw;
	if(bufferProducer!=NULL){
		anw=new Surface(bufferProducer,true/*controlledByApp*/);
		status_t err=native_window_api_connect(anw.get(),NATIVE_WINDOW_API_MEDIA);
		if(err!=OK){
			reset();
			disconnectNativeWindow();
			return err;
		}
	}
	status_t err=p->setVideoSurfaceTexture(bufferProducer);
	disconnectNativeWindow();
	mConnectedWindow=anw;
	if(err==OK)
		mConnectedWindwoBinder=binder;
	else
		disconnectNativeWindow();
	return err;
}
```
# 2.Subtitle