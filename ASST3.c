/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *Author_Info = "//Doug Griffin\n //CSCI 340 - Operating Systems\n"; //Wanted to make it so I could include a command that would add this at the beginning of a file.

static int hello_getattr(const char *path, struct stat *stbuf)
{
        int res = 0;

        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else if (strcmp(path, hello_path) == 0) {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen(hello_str);
        } else
                res = -ENOENT;

        return res;
}

int hello_chmod(const char *path, mode_t mode) //to change permissions
{
    int res = 0;
    res = chmod(path, 777);
    return res;
}

int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi) //to create a file
{
    int res = 0;
    int fd;
    fd = creat(path, mode);
    fi->fh = fd;
    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi) //readdir unchanged from hello
{
        (void) offset;
        (void) fi;

        if (strcmp(path, "/") != 0)
                return -ENOENT;

        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, hello_path + 1, NULL, 0);

        return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi) //opening a file
{
        int res = 0;
        int fd;
        fd = open(path, fi->flags);
        
        fi->fh = fd;
        

        return res;
}

static int hello_mkdir(const char *path, mode_t mode) //making a directory
{
    int res = 0;
    res = mkdir(path, mode);
    return res;
}

static int hello_mknod(const char *path, mode_t mode, dev_t dev) //making a file node
{
    int res = 0;
    res = mknod(path, mode, dev);
    return res;
}
    
static int hello_read(const char *path, char *buf, size_t size, off_t offset, //reading hello
                      struct fuse_file_info *fi)
{
        size_t len;
        (void) fi;
        if(strcmp(path, hello_path) != 0)
                return -ENOENT;

        len = strlen(hello_str);
        if (offset < len) {
                if (offset + size > len)
                        size = len - offset;
                memcpy(buf, hello_str + offset, size);
        } else
                size = 0;

        return size;
}

static int hello_write(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) //writing to a file
{
    size_t len;
    (void) fi;
    len = strlen(hello_str);
    
    size = pwrite(fi->fh, Author_Info, size, offset);
    return size;
}
static struct fuse_operations hello_oper = {
        .getattr        = hello_getattr,
        .readdir        = hello_readdir,
        .open           = hello_open,
        .read           = hello_read,
        .write          = hello_write,
        .chmod          = hello_chmod,
        .mknod          = hello_mknod,
        .mkdir          = hello_mkdir,
};

int main(int argc, char *argv[])
{
        return fuse_main(argc, argv, &hello_oper, NULL);
}


// syntax highlighted by Code2HTML, v. 0.9.1 