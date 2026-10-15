#include "uart_cmd_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

bool parse_pipegen_frame(const char *line_buf, pipegen_cmd_t *cmd_out) {
    if (!line_buf || !cmd_out) return false;

    memset(cmd_out, 0, sizeof(pipegen_cmd_t));

    // Trim leading whitespace
    while (*line_buf && isspace((unsigned char)*line_buf)) {
        line_buf++;
    }
    if (!*line_buf) return false;

    // Check for "CLEAR"
    if (strncmp(line_buf, "CLEAR", 5) == 0) {
        const char *after = line_buf + 5;
        while (*after && isspace((unsigned char)*after)) {
            after++;
        }
        if (*after == '\0' || *after == '\r' || *after == '\n') {
            cmd_out->type = CMD_TYPE_CLEAR;
            return true;
        }
    }

    // Check for "ALERT:<ZONE>:<REASON>"
    if (strncmp(line_buf, "ALERT:", 6) == 0) {
        const char *zone_start = line_buf + 6;
        const char *colon = strchr(zone_start, ':');
        if (!colon) return false;

        size_t zone_len = (size_t)(colon - zone_start);
        if (zone_len == 0 || zone_len >= sizeof(cmd_out->zone)) return false;

        strncpy(cmd_out->zone, zone_start, zone_len);
        cmd_out->zone[zone_len] = '\0';

        const char *reason_start = colon + 1;
        // Find end of reason (before newline, carriage return, or whitespace)
        size_t r_len = 0;
        while (reason_start[r_len] && reason_start[r_len] != '\r' &&
               reason_start[r_len] != '\n') {
            r_len++;
        }
        // Trim trailing whitespace from reason
        while (r_len > 0 && isspace((unsigned char)reason_start[r_len - 1])) {
            r_len--;
        }
        if (r_len == 0 || r_len >= sizeof(cmd_out->reason)) return false;

        strncpy(cmd_out->reason, reason_start, r_len);
        cmd_out->reason[r_len] = '\0';

        cmd_out->type = CMD_TYPE_ALERT;
        return true;
    }

    cmd_out->type = CMD_TYPE_UNKNOWN;
    return false;
}
