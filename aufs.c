/*
 * Copyright (c) 2017 Carl Harris Jr, Michael Irwin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mount.h>

#include "dstring.h"
#include "aufs.h"

#define SOURCE_NAME         "aufs"
#define AUFS_TYPE           "aufs"
#define AUFS_IMAGE_PATH     "/image"
#define AUFS_CONFIG_PATH    "/config"
#define AUFS_CONTAINER_PATH "/container"


static String * aufs_opts();
static void append_layers(String *opts, const char *aufs_path);

int mount_aufs(const char *aufs_path, const char *root_path)
{
  String *opts = aufs_opts(aufs_path);

  int rc = mount(SOURCE_NAME, root_path, AUFS_TYPE, 0, dstr_text(opts));
  if (rc == 0) {
    rc = mount(NULL, root_path, NULL, MS_PRIVATE, NULL);
  }

  dstr_free(opts);

  return rc;
}

static String * aufs_opts(const char *aufs_path) {
  String *opts = dstr_init("dirs=");
  dstr_append(opts, aufs_path);
  dstr_append(opts, AUFS_CONTAINER_PATH);
  dstr_append(opts, "=rw:");
  dstr_append(opts, aufs_path);
  dstr_append(opts, AUFS_CONFIG_PATH);
  dstr_append(opts, "=ro");
  append_layers(opts, aufs_path);
  return opts;
}

static void append_layers(String *opts, const char *aufs_path)
{
  struct dirent **namelist;
  String *path = dstr_init(aufs_path);
  dstr_append(path, AUFS_IMAGE_PATH);

  int n = scandir(dstr_text(path), &namelist, NULL, versionsort);
  dstr_free(path);

  if (n < 0) {
    perror("scandir");
    exit(EXIT_FAILURE);
  }
  while (n--) { 
    char *name = namelist[n]->d_name;
    if (strncmp(name, ".", 2) != 0
        && strncmp(name, "..", 2) != 0) {
      dstr_append(opts, ":");
      dstr_append(opts, aufs_path);
      dstr_append(opts, AUFS_IMAGE_PATH);
      dstr_append(opts, "/");
      dstr_append(opts, name);
      dstr_append(opts, "=ro");
    }
    free(namelist[n]);
  }
}
