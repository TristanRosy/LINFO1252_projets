#include <string.h>
#include <stdio.h>
#include <errno.h>
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

    lseek(tar_fd, 0, SEEK_SET);

    tar_header_t header;
    int count = 0;

    while (read(tar_fd, &header, 512)) {

        if (strncmp(header.magic, TMAGIC, 6) != 0) {
            return -1;
        }

        if (strncmp(header.version, TVERSION, 2) != 0) {
            return -2;
        }

        int sum = 0;
        char* iter = (char*) &header;
        for (int i = 0; i < 512; i++) {
            if (i < 148 || 156 <= i) sum += iter[i];
            else sum += ' ';
        }

        if (TAR_INT(header.chksum) != sum) {
            return -3;
        }

        int file_size = TAR_INT(header.size);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
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

    lseek(tar_fd, 0, SEEK_SET);

    tar_header_t header;

    while (read(tar_fd, &header, 512)){
        if (strncmp(path, header.name, strlen(path)) == 0) {
            return 1;
        }

        int file_size = TAR_INT(header.size);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
    }
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

    lseek(tar_fd, 0, SEEK_SET);

    tar_header_t header;

    while (read(tar_fd, &header, 512)) {
        if (strncmp(header.name, path, strlen(path)) == 0) {
            if (header.typeflag == typeflag) {
                return 1;
            } else {
                return 0;
            }
        }

        int file_size = TAR_INT(header.size);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
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
    lseek(tar_fd, 0, SEEK_SET);

    tar_header_t header;
    size_t count = 0;

    while (read(tar_fd, &header, 512)) {
        if (strncmp(path, header.name, strlen(path)) == 0) {
            size_t entry_len = strnlen(header.name, 100) + 1; // Including null terminator
            entries[count] = malloc(entry_len);
            strcpy(entries[count], header.name);
            count++;
        }

        int file_size = TAR_INT(header.size);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
    }

    *no_entries = count;
    return count;
}

/**
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from. If the entry is a symlink, it must be resolved to its linked-to entry.
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
 */
 
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    lseek(tar_fd, 0, SEEK_SET);

    tar_header_t header;
    int max_loops = 1000;  // Set a maximum number of loops to avoid an infinite loop

    while (read(tar_fd, &header, sizeof(tar_header_t)) > 0 && max_loops-- > 0) {
        if (strncmp(path, header.name, strlen(path)) == 0) {
            if (header.typeflag == '0') {
                // Regular file entry
                int64_t file_size = TAR_INT(header.size);

                if (offset >= file_size) {
                    return -2; // Offset is outside the file total length
                }

                lseek(tar_fd, offset + (offset / 512 * 512), SEEK_CUR);

                ssize_t bytes_to_read = *len;
                if (offset + *len > file_size) {
                    bytes_to_read = file_size - offset;
                }

                ssize_t bytes_read = read(tar_fd, dest, bytes_to_read);

                if (bytes_read < 0) {
                    return -1; // Error reading file
                }

                *len = bytes_read;

                return (file_size - offset - bytes_read) > 0 ? file_size - offset - bytes_read : 0;
            } else if (header.typeflag == '2') {
                // Symlink entry
                char linked_file[256]; // Adjust the size as needed
                ssize_t symlink_size = TAR_INT(header.size);

                // Read the symlink target path
                if (read(tar_fd, linked_file, symlink_size) != symlink_size) {
                    return -1; // Error reading symlink target
                }
                linked_file[symlink_size] = '\0'; // Null-terminate the string

                int is_symlink_result = is_symlink(tar_fd, linked_file);
                if (is_symlink_result != 0) {
                    // Check if it's a file
                    int is_file_result = is_file(tar_fd, linked_file);
                    if (is_file_result != 0) {
                        // If it's a file, read it
                        return read_file(tar_fd, linked_file, offset, dest, len);
                    } else {
                        // Unsupported symlink type or file not found
                        return -1;
                    }
                } else {
                    // Handle symlink recursively
                    size_t linked_file_len = strlen(linked_file);
                    if (linked_file_len < sizeof(linked_file) - 1 && linked_file[linked_file_len - 1] != '/') {
                        // Add a '/' at the end of the path if it's a symbolic link to a directory
                        linked_file[linked_file_len] = '/';
                        linked_file[linked_file_len + 1] = '\0';
                    }
                    return read_file(tar_fd, linked_file, offset, dest, len);
                }
            }
        }

        // Skip to the next header block
        int64_t file_size = TAR_INT(header.size);
        int remaining_blocks = (file_size + 511) / 512;
        lseek(tar_fd, remaining_blocks * 512, SEEK_CUR);
    }

    return -1; // No entry at the given path or the entry is not a file
}

