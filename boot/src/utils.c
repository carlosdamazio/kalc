// memcmp implementation
int memcmp(const void *aptr, const void *bptr, int n)
{
	const unsigned char *a = aptr, *b = bptr;
	for (int i = 0; i < n; i++) {
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}

