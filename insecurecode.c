static PyObject *
sock_recvfrom_into(PySocketSockObject *s, PyObject *args, PyObject* kwds)
{
	static char *kwlist[] = {"buffer", "nbytes", "flags", 0};

	int recvlen = 0, flags = 0;
	ssize_t readlen;
	Py_buffer buf;
	int buflen;

	PyObject *addr = NULL;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "w*|ii:recvfrom_into",
					 kwlist, &buf,
					 &recvlen, &flags))
		return NULL;
	buflen = buf.len;
	assert(buf.buf != 0 && buflen > 0);

	if (recvlen < 0) {
		PyErr_SetString(PyExc_ValueError,
				"negative buffersize in recvfrom_into");
		goto error;
	}
	if (recvlen == 0) {
		/* If nbytes was not specified, use the buffer's length */
		recvlen = buflen;
	}

	readlen = sock_recvfrom_guts(s, buf.buf, recvlen, flags, &addr);
	if (readlen < 0) {
		/* Return an error */
		goto error;
	}

	PyBuffer_Release(&buf);
	/* Return the number of bytes read and the address.  Note that we do
	   not do anything special here in the case that readlen < recvlen. */
	return Py_BuildValue("lN", readlen, addr);

  error:
	Py_XDECREF(addr);
	PyBuffer_Release(&buf);
	return NULL;
}
