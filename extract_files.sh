#!/bin/bash

OUTPUT_FILE="extract.txt"

echo "=== EXTRACTION DE FICHIERS DU PROJET ALgo ===" > "$OUTPUT_FILE"
echo >> "$OUTPUT_FILE"

# Liste des fichiers standards
FILES=("algo.y" "algo.l" "sample.algo" "Makefile")
TEX_FILES=$(ls *.tex 2>/dev/null)

# Fichiers principaux
for file in "${FILES[@]}"; do
    if [[ -f "$file" ]]; then
        {
            echo "--------------------"
            echo "📄 Fichier : $file"
            echo "--------------------"
            cat "$file"
            echo -e "\n"
        } >> "$OUTPUT_FILE"
    fi
done

# Fichiers .tex
for file in $TEX_FILES; do
    {
        echo "--------------------"
        echo "📄 Fichier TEX : $file"
        echo "--------------------"
        cat "$file"
        echo -e "\n"
    } >> "$OUTPUT_FILE"
done

echo "✅ Contenu extrait dans $OUTPUT_FILE"