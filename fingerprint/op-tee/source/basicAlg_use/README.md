     
在整个包中有将该TA集成到OP-TEE中的patch

由于在xtest中对于算法接口的测试都是通过一步一步来完成，而在基础算法的具体使用时并不会采取该方式，而是将数据通过CA传递給TA，
然后让TA完成一次完成的算法操作，然后返回加密，解密，签名，验签的
结果到CA中。
	本TA中包含了RSA1024/RSA2048/HMAC/SHA1/SHA256/AES/PBKDF2/RAMDON等
算法的具体使用操作。只要在CA端将需要使用算法进行加密的数据传递給
TEE并执行之后就能得到使用该算法操作后的结果。下面介绍在REE侧使用
运行CA调用TA来完成上述算法计算的具体操作，主要是在linux中的shell
中运行指令。（在qemu+OP-TEE的环境下测试通过）算法操作使用到的key
都会保存在TA中。
	在使用过程中遇见任何问题可以联系:shuaifengyun@126.com
1. RSA1024算法的使用
	encrypt operation(加密操作,使用rsa1024 public key加密数据)指令如下：
		basicAlgUse rsa1024 enc
	decrypt operation(解密操作,使用rsa1024 private key解密数据)指令如下：
		basicAlgUse rsa1024 dec
	sign operation(签名操作,使用rsa1024 private key加密数据)指令如下：
		basicAlgUse rsa1024 sign
	verify operation(验签操作,使用rsa1024 public key解密数据)指令如下：
		basicAlgUse rsa1024 verify

2. RSA2048算法的使用
	encrypt operation(加密操作,使用rsa2048 public key加密数据)指令如下：
		basicAlgUse rsa2048 enc
	decrypt operation(解密操作,使用rsa2048 private key解密数据)指令如下：
		basicAlgUse rsa2048 dec
	sign operation(签名操作,使用rsa2048 private key加密数据)指令如下：
		basicAlgUse rsa2048 sign
	verify operation(验签操作,使用rs2048 public key解密数据)指令如下：
		basicAlgUse rsa2048 verify

3. RANDOM的使用
	让TA产生指定长度的随机数指令如下(num为要产生的随机数的长度)：
	basicAlgUse random [num]

4. AES算法的使用
	AES的CBC模式进行加密操作指令如下：
		basicAlgUse aes enc cbc
	AES的ECB模式进行加密操作指令如下：
		basicAlgUse aes enc ecb
	AES的CTR模式进行加密操作指令如下：
		basicAlgUse aes enc ctr
	AES的CTS模式进行加密操作指令如下：
		basicAlgUse aes enc cts
	AES的CBC模式进行解密操作指令如下：
		basicAlgUse aes dec cbc
	AES的ECB模式进行解密操作指令如下：
		basicAlgUse aes dec ecb
	AES的CTR模式进行解密操作指令如下：
		basicAlgUse aes dec ctr
	AES的CTS模式进行解密操作指令如下：
		basicAlgUse aes dec cts

5. HMAC的使用
	使用HMAC产生指定长度的数据（password和salt在TA中设定）的指令如下
	(len是指定返回给CA的数据的长度，count是指定计算的循环次数)：
		basicAlgUse hmac [len] [count]

6. BASE64的使用
	使用base64进行加密(在TA中使用软件方案实现)指令如下：
		basicAlgUse base64 enc
	使用base64进行解密(在TA中使用软件方案实现)指令如下：
		basicAlgUse base64 dec

7. PBKDF2的使用
	使用PBKDF2，一般在generate key时被使用（num为要返回给CA的数据的长度）
	pbkdf2算法中使用的salt, passworkd, count值都在TA中被设定，指令如下：
		basicAlgUse pbkdf [num]
