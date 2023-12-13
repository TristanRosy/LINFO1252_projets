#include <string.h>
#include <stdio.h>
#include "lib_tar.h"

/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd) {

    int count = 0;


    tar_header_t header;
    ssize_t blocks = read(tar_fd, &header, 512);

    while (blocks == 512) {


        if (strncmp(header.magic, TMAGIC, 6) != 0) {
            printf("test\n");
            return -1;
        }

        if (strncmp(header.version, TVERSION, 2) != 0) {
            return -2;
        }

        int sum = 0;
        char* iter = (char*) &header;
        for (int i = 0; i < 512; i++) {
            if (i < 148 || 156 < i) sum += iter[i];
        }

        if (atoi(header.chksum) != (uint32_t) sum){
            return -3;
        }

        lseek(tar_fd, atoi(header.size), SEEK_CUR);
        blocks = read(tar_fd, &header, 512);
        count++;
    }


    return count;
}

/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path) {
    return 0;
}


/**
 * Création d'une fonction Helper pour les fonctions is_dir, is_file, is_symlink
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @param typeflag the flag corresponding to dir/file/symlink
 */

int check_entry_type(int tar_fd, char *path, char typeflag) {
    int found = 0;
    tar_header_t header;
    ssize_t blocks = read(tar_fd, &header, 512);
    while (blocks == 512) {
        if (strncmp(header.name, path, strlen(path)) == 0) {
            if (header.typeflag == typeflag) {
                return 1;
            } else {
                return 0;
            }
        }
        int file_size = strtol(header.size, NULL, 8);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
        blocks = read(tar_fd, &header, 512);
    }
    return found;
}


/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path) {
    return check_entry_type(tar_fd, path, '5');
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path) {
    return check_entry_type(tar_fd, path, '0');
}



/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path) {
    return check_entry_type(tar_fd, path, '2');
}


/**
 * Lists the entries at a given path in the archive.
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries) {
    return 0;
}

/** ANTOINE
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    return 0;
}