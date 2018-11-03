打开了安全文件之后就可以对打开的安全文件中的数据执行读写操作了。在TA中对安全文件进行读写操作是分别通过调用`TEE_ReadObjectData`和`TEE_WriteObjectData`函数来实现的，调用之后最终会切换到OP-TEE的kernel space中调用对应的读写接口`syscall_storage_obj_read`和`syscall_storage_obj_write`函数来完成对安全文件中数据的读写操作。

# 1.安全文件中数据的读取(read)操作
在TA中调用读取接口之后在kernel space将会调用syscall_storage_obj_read函数来实现对安全文件中数据的读取操作。该函数的执行过程中同样跟open操作一样，首先会获取TA的session和context并检查权限。然后调用ree_fs_read函数来实现读取数据操作。该函数内容如下；

```c
static TEE_Result ree_fs_read(struct tee_file_handle *fh, size_t pos,
			      void *buf, size_t *len)
{
	TEE_Result res;
 
/* 传入的post是要读取的数据在安全文件中数据区域中的起始位置，
可以通过object的seek函数改变buf为读取到的数据存放的地址 */
	mutex_lock(&ree_fs_mutex);	//互斥的lock操作
	res = ree_fs_read_primitive(fh, pos, buf, len);	//进入读取操作函数
	mutex_unlock(&ree_fs_mutex);	//互斥的unlock操作
 
	return res;
}
 
static TEE_Result ree_fs_read_primitive(struct tee_file_handle *fh, size_t pos,
					void *buf, size_t *len)
{
	TEE_Result res;
	int start_block_num;
	int end_block_num;
	size_t remain_bytes;
	uint8_t *data_ptr = buf;
	uint8_t *block = NULL;
	struct tee_fs_fd *fdp = (struct tee_fs_fd *)fh;
	struct tee_fs_htree_meta *meta = tee_fs_htree_get_meta(fdp->ht);
 
/* 判定需要读取的长度是否被满足 */
	remain_bytes = *len;
	if ((pos + remain_bytes) < remain_bytes || pos > meta->length)
		remain_bytes = 0;
	else if (pos + remain_bytes > meta->length)
		remain_bytes = meta->length - pos;
 
	*len = remain_bytes;
 
	if (!remain_bytes) {
		res = TEE_SUCCESS;
		goto exit;
	}
 
/* 计算出读取位置数据安全文件数据区域中的哪个block，并根据需要读取的数据
长度计算出需要读取的数据尾部在那个block */
	start_block_num = pos_to_block_num(pos);
	end_block_num = pos_to_block_num(pos + remain_bytes - 1);
 
/* 分配buffer保存读取的block数据 */
	block = malloc(BLOCK_SIZE);
	if (!block) {
		res = TEE_ERROR_OUT_OF_MEMORY;
		goto exit;
	}
 
/* 使用while循环开始读取数据，当查出en_block_num时表示读取操作完成 */
	while (start_block_num <= end_block_num) {
/* 计算出需要读取的文件在该block中的offset */
		size_t offset = pos % BLOCK_SIZE;	
 
/* 计算需要读取的长度 */
		size_t size_to_read = MIN(remain_bytes, (size_t)BLOCK_SIZE);
 
		if (size_to_read + offset > BLOCK_SIZE)
			size_to_read = BLOCK_SIZE - offset;
 
/* 读取block number编号为start_block_number的数据块的数据，在
tee_fs_htree_read_block函数中将会根据start_block_number找到该block
对应的node，获取到该block加解密使用的iv，然后使用iv和该文件fek解密从
安全文件中读取的数据获得明文的数据 */
		res = tee_fs_htree_read_block(&fdp->ht, start_block_num, block);
		if (res != TEE_SUCCESS)
			goto exit;
 
/* 按照offset和size_to_read拷贝读出的明文数据到buffer中 */
		memcpy(data_ptr, block + offset, size_to_read);
 
/* 计算偏移 */
		data_ptr += size_to_read;
		remain_bytes -= size_to_read;
		pos += size_to_read;
 
		start_block_num++;
	}
	res = TEE_SUCCESS;
exit:
	free(block);
	return res;
}
```
# 2.安全文件中数据写入（write）操作
在TA中调用读取接口之后在kernel space将会调用syscall_storage_obj_write函数来实现对安全文件中数据的读取操作。该函数的执行过程中同样跟open操作一样，首先会获取TA的session和context并检查权限。然后调用ree_fs_write函数来实现读取数据操作。该函数内容如下：

```c
static TEE_Result ree_fs_write(struct tee_file_handle *fh, size_t pos,
			       const void *buf, size_t len)
{
	TEE_Result res;
	struct tee_fs_dirfile_dirh *dirh = NULL;
	struct tee_fs_fd *fdp = (struct tee_fs_fd *)fh;
 
	mutex_lock(&ree_fs_mutex);
 
/* dirf.db文件操作，由于已经在open中执行过，故不会重复打开dirf.db文件 */
	res = get_dirh(&dirh);
	if (res)
		goto out;
/* 将数据写入到安全文件中，写入之前会对数据进行加密操作，执行的时候首先会将
牵扯到的block中的数据全部读出，然后将需要被写入的数据替换掉对应的区域，然后再
调用tee_fs_htree_write_block函数将数据进行加密操作后写入到安全文件中 */
	res = ree_fs_write_primitive(fh, pos, buf, len);
	if (res)
		goto out;
 
/* 更新整个安全文件的node tree信息和head部分的数据 */
	res = tee_fs_htree_sync_to_storage(&fdp->ht, fdp->dfh.hash);
	if (res)
		goto out;
 
/* 更新dirf.db文件中该安全文件对应的dirfile_entry结构体数据 */
	res = tee_fs_dirfile_update_hash(dirh, &fdp->dfh);
	if (res)
		goto out;
 
/* 更新相关hash值 */
	res = tee_fs_dirfile_commit_writes(dirh);
out:
	put_dirh(dirh);
	mutex_unlock(&ree_fs_mutex);
 
	return res;
}
```