#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

const int PATH_MAX = 30;

struct fs_state
{
    char* root_dir;
    char* file_found;
};

void get_full_path(char full_path[PATH_MAX], const char *path)
{
    strcpy(full_path, root_dir);
    strncat(full_path, path, PATH_MAX);
}

int search_getattr(const char *path, struct stat *stbuf)
{
        int res = 0;
        char full_path[PATH_MAX];
        
        get_full_path(full_path, path);
        
        res = lstat(full_path, stbuf);
        if(res != 0)
        {
            return -ENOENT;
        }
        
        return res;
}

static int search_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
        (void) offset;
        (void) fi;

        if (strcmp(path, "/") != 0)
                return -ENOENT;

        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, search_path + 1, NULL, 0);

        return 0;
}

static int search_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
        int res = 0;
        
        res = pread(fi->fh, buf, size, offset); //use pread so we don't mess with the file pointer.
        return res;
}


static int search_open(const char *path, struct fuse_file_info *fi)
{
        int res = 0;
        int fd;
        char full_path[PATH_MAX];
        
        get_full_path(full_path, path);
        fd = open(full_path, fi->flags);
        if(fd < 0)
        {
            res = -ENOENT;
        }
        
        fi->fh = fd;
        
        return res;
}

static struct fuse_operations search_dir = {
        .getattr        = search_getattr,
        .readdir        = search_readdir,
        .open           = search_open,
        .read           = search_read,
};
        
int main(int argc, char *argv[])
{
        struct fs_state *state;
        state = malloc(sizeof(struct fs_state));
   
        state->root_dir = realpath(argv[1], NULL); //set the root directory to the first argument(which should be the mount point)
        state->file_found = realpath(argv[2], NULL); // set file_found to the second argument, which is the file we will be looking for.
        return fuse_main(argc, argv, &search_dir, NULL);
}