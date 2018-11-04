global-incdirs-y += include
#global-incdirs-y += ../host/include
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaAes.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaHash.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaPbkdf2.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaDebug.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaOthre.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaRsa.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaHandle.c
#srcs-$(CFG_CRYPTO_VERIFY_TA) += CryptoVerifyTaEntry.c

srcs-y += CryptoVerifyTaEntry.c
srcs-y += CryptoVerifyTaHandle.c
srcs-y += CryptoVerifyTaAes.c
srcs-y += CryptoVerifyTaHash.c
srcs-y += CryptoVerifyTaPbkdf2.c
srcs-y += CryptoVerifyTaDebug.c
srcs-y += CryptoVerifyTaOther.c
srcs-y += CryptoVerifyTaRsa.c



# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
