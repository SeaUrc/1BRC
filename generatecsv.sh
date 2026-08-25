#!/usr/bin/env bash

OUTPUT_FILE="measurements.txt"
TOTAL_ROWS=1000000000
NUM_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
ROWS_PER_JOB=$((TOTAL_ROWS / NUM_JOBS))

echo "Generating $TOTAL_ROWS rows into '$OUTPUT_FILE' using $NUM_JOBS parallel jobs..."

generate_chunk() {
    local count=$1
    local seed=$2
    local outfile=$3

    awk -v rows="$count" -v seed="$seed" '
    BEGIN {
        srand(seed)

        n = 0
        c[++n]="Detroit";          m[n]=26.4;  s[n]=6.07
        c[++n]="Pontiac";          m[n]=23.1;  s[n]=3.80
        c[++n]="Ann Arbor";        m[n]=27.0;  s[n]=8.28
        c[++n]="Grand Rapids";     m[n]=11.4;  s[n]=4.50
        c[++n]="Muskegon";         m[n]=13.0;  s[n]=4.57
        c[++n]="Lansing";          m[n]=23.18; s[n]=8.40
        c[++n]="Flint";            m[n]=25.5;  s[n]=4.76
        c[++n]="Saginaw";          m[n]=27.43; s[n]=6.90
        c[++n]="Traverse City";    m[n]=10.8;  s[n]=4.25
        c[++n]="Alpena";           m[n]=20.48; s[n]=3.20
        c[++n]="Sault Ste. Marie"; m[n]=24.8;  s[n]=7.73
        c[++n]="Ironwood";         m[n]=20.83; s[n]=4.20
        c[++n]="Iron Mountain";    m[n]=25.8;  s[n]=8.93
        c[++n]="Escanaba";         m[n]=12.73; s[n]=3.40
        c[++n]="Marquette";        m[n]=17.8;  s[n]=6.20
        c[++n]="Houghton";         m[n]=22.0;  s[n]=6.80
        c[++n]="Chicago";          m[n]=13.6;  s[n]=6.54
        c[++n]="Los Angeles";      m[n]=15.14; s[n]=3.20
        c[++n]="San Francisco";    m[n]=19.5;  s[n]=3.82
        c[++n]="New York";         m[n]=25.86; s[n]=3.90
        c[++n]="Buffalo";          m[n]=16.3;  s[n]=7.48
        c[++n]="Boston";           m[n]=17.96; s[n]=7.10
        c[++n]="Dulles";           m[n]=15.38; s[n]=6.80
        c[++n]="Atlanta";          m[n]=27.1;  s[n]=3.17
        c[++n]="Miami";            m[n]=15.34; s[n]=3.30
        c[++n]="New Orleans";      m[n]=24.9;  s[n]=7.34
        c[++n]="Dallas";           m[n]=11.87; s[n]=5.80
        c[++n]="Houston";          m[n]=11.8;  s[n]=6.23
        c[++n]="Denver";           m[n]=11.75; s[n]=8.60
        c[++n]="Phoenix";          m[n]=24.7;  s[n]=4.02
        c[++n]="Seattle";          m[n]=14.62; s[n]=4.20
        c[++n]="Portland";         m[n]=18.6;  s[n]=6.27
        c[++n]="Salt Lake City";   m[n]=10.51; s[n]=5.20
        c[++n]="Las Vegas";        m[n]=19.5;  s[n]=4.93
        c[++n]="Honolulu";         m[n]=10.81; s[n]=4.90
        c[++n]="Anchorage";        m[n]=16.8;  s[n]=4.30
        c[++n]="Vancouver";        m[n]=22.36; s[n]=7.20
        c[++n]="Toronto";          m[n]=22.3;  s[n]=7.05
        c[++n]="Montreal";         m[n]=17.55; s[n]=6.60
        c[++n]="London";           m[n]=14.5;  s[n]=3.01
        c[++n]="Tokyo";            m[n]=23.25; s[n]=5.60
        c[++n]="Sydney";           m[n]=23.8;  s[n]=3.27

        for (i = 0; i < rows; i++) {
            idx = int(rand() * n) + 1

            u1 = rand()
            while (u1 == 0) u1 = rand()
            u2 = rand()
            z = sqrt(-2.0 * log(u1)) * cos(6.283185307179586 * u2)

            temp = m[idx] + (z * s[idx])
            printf "%s;%.1f\n", c[idx], temp
        }
    }' > "$outfile"
}

export -f generate_chunk

# Create temporary directory for chunks
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

# Run parallel jobs writing to isolated files
for ((i=1; i<=NUM_JOBS; i++)); do
    generate_chunk "$ROWS_PER_JOB" "$((RANDOM * 1000 + i))" "$TMP_DIR/chunk_$i.txt" &
done

wait

# Merge intermediate files cleanly
cat "$TMP_DIR"/chunk_*.txt > "$OUTPUT_FILE"

echo "Done! Wrote 1 billion rows to '$OUTPUT_FILE'."
