// kg_lookup.h - Bare-metal KG lookup for VEGA RISC-V FPGA
// Single-header, constexpr hash table, zero allocation, no STL, no exceptions.
// Compile: g++ -std=c++17 -fno-exceptions -fno-rtti -O2

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Include generated data
#include "kg_data.h"

// ============================================================================
// Configuration
// ============================================================================

constexpr uint16_t KG_MAX_PROMPT_CHARS = 100;
constexpr uint8_t KG_MAX_TOKENS = 20;
constexpr uint16_t KG_HASH_TABLE_SIZE = 64; // Must be power of 2 for fast modulo

// ============================================================================
// Result Structure
// ============================================================================

struct KgResult
{
    const uint8_t *classes; // Pointer into .rodata (or nullptr)
    uint8_t count;          // Number of classes
    bool matched;           // True if a token was found
};

// ============================================================================
// Hashing (djb2) - constexpr compatible
// ============================================================================

constexpr uint32_t kg_djb2_hash(const char *str, uint8_t len)
{
    uint32_t hash = 5381;
    for (uint8_t i = 0; i < len; ++i)
    {
        hash = ((hash << 5) + hash) ^ (uint8_t)str[i]; // hash * 33 ^ char
    }
    return hash;
}

// ============================================================================
// Lookup Table Entry (populated from kg_data.h tokens)
// ============================================================================

struct KgEntry
{
    uint32_t hash;
    const char *token; // For collision verification
    uint8_t token_len;
    const uint8_t *classes; // Pointer to KG_{TOKEN}_CLASSES[]
    uint8_t count;          // KG_{TOKEN}_LEN
};

// ============================================================================
// Static Hash Table (constexpr populated)
// ============================================================================

// Populate via X-macro pattern: define entries for each known token
// This is manually generated from kg_data.h tokens
constexpr KgEntry KG_LOOKUP_TABLE[49] = {
    {kg_djb2_hash("bark", 4), "bark", 4, KG_BARK_CLASSES, KG_BARK_LEN},
    {kg_djb2_hash("capture", 7), "capture", 7, KG_CAPTURE_CLASSES, KG_CAPTURE_LEN},
    {kg_djb2_hash("chug", 4), "chug", 4, KG_CHUG_CLASSES, KG_CHUG_LEN},
    {kg_djb2_hash("chugged", 7), "chugged", 7, KG_CHUGGED_CLASSES, KG_CHUGGED_LEN},
    {kg_djb2_hash("clean", 5), "clean", 5, KG_CLEAN_CLASSES, KG_CLEAN_LEN},
    {kg_djb2_hash("close", 5), "close", 5, KG_CLOSE_CLASSES, KG_CLOSE_LEN},
    {kg_djb2_hash("coffee", 6), "coffee", 6, KG_COFFEE_CLASSES, KG_COFFEE_LEN},
    {kg_djb2_hash("consume", 7), "consume", 7, KG_CONSUME_CLASSES, KG_CONSUME_LEN},
    {kg_djb2_hash("consuming", 9), "consuming", 9, KG_CONSUMING_CLASSES, KG_CONSUMING_LEN},
    {kg_djb2_hash("cook", 4), "cook", 4, KG_COOK_CLASSES, KG_COOK_LEN},
    {kg_djb2_hash("drink", 5), "drink", 5, KG_DRINK_CLASSES, KG_DRINK_LEN},
    {kg_djb2_hash("drink_hot", 9), "drink_hot", 9, KG_DRINK_HOT_CLASSES, KG_DRINK_HOT_LEN},
    {kg_djb2_hash("drive", 5), "drive", 5, KG_DRIVE_CLASSES, KG_DRIVE_LEN},
    {kg_djb2_hash("eat", 3), "eat", 3, KG_EAT_CLASSES, KG_EAT_LEN},
    {kg_djb2_hash("eat_snack", 9), "eat_snack", 9, KG_EAT_SNACK_CLASSES, KG_EAT_SNACK_LEN},
    {kg_djb2_hash("fetch", 5), "fetch", 5, KG_FETCH_CLASSES, KG_FETCH_LEN},
    {kg_djb2_hash("grab", 4), "grab", 4, KG_GRAB_CLASSES, KG_GRAB_LEN},
    {kg_djb2_hash("hold", 4), "hold", 4, KG_HOLD_CLASSES, KG_HOLD_LEN},
    {kg_djb2_hash("look", 4), "look", 4, KG_LOOK_CLASSES, KG_LOOK_LEN},
    {kg_djb2_hash("open", 4), "open", 4, KG_OPEN_CLASSES, KG_OPEN_LEN},
    {kg_djb2_hash("openup", 6), "openup", 6, KG_OPENUP_CLASSES, KG_OPENUP_LEN},
    {kg_djb2_hash("pet", 3), "pet", 3, KG_PET_CLASSES, KG_PET_LEN},
    {kg_djb2_hash("petting", 7), "petting", 7, KG_PETTING_CLASSES, KG_PETTING_LEN},
    {kg_djb2_hash("photo", 5), "photo", 5, KG_PHOTO_CLASSES, KG_PHOTO_LEN},
    {kg_djb2_hash("pick", 4), "pick", 4, KG_PICK_CLASSES, KG_PICK_LEN},
    {kg_djb2_hash("pick_up", 7), "pick_up", 7, KG_PICK_UP_CLASSES, KG_PICK_UP_LEN},
    {kg_djb2_hash("play", 4), "play", 4, KG_PLAY_CLASSES, KG_PLAY_LEN},
    {kg_djb2_hash("pull", 4), "pull", 4, KG_PULL_CLASSES, KG_PULL_LEN},
    {kg_djb2_hash("pulled", 6), "pulled", 6, KG_PULLED_CLASSES, KG_PULLED_LEN},
    {kg_djb2_hash("push", 4), "push", 4, KG_PUSH_CLASSES, KG_PUSH_LEN},
    {kg_djb2_hash("pushes", 6), "pushes", 6, KG_PUSHES_CLASSES, KG_PUSHES_LEN},
    {kg_djb2_hash("read", 4), "read", 4, KG_READ_CLASSES, KG_READ_LEN},
    {kg_djb2_hash("ride", 4), "ride", 4, KG_RIDE_CLASSES, KG_RIDE_LEN},
    {kg_djb2_hash("shut", 4), "shut", 4, KG_SHUT_CLASSES, KG_SHUT_LEN},
    {kg_djb2_hash("sip", 3), "sip", 3, KG_SIP_CLASSES, KG_SIP_LEN},
    {kg_djb2_hash("sip_hot", 7), "sip_hot", 7, KG_SIP_HOT_CLASSES, KG_SIP_HOT_LEN},
    {kg_djb2_hash("sips", 4), "sips", 4, KG_SIPS_CLASSES, KG_SIPS_LEN},
    {kg_djb2_hash("sit", 3), "sit", 3, KG_SIT_CLASSES, KG_SIT_LEN},
    {kg_djb2_hash("snack", 5), "snack", 5, KG_SNACK_CLASSES, KG_SNACK_LEN},
    {kg_djb2_hash("snacking", 8), "snacking", 8, KG_SNACKING_CLASSES, KG_SNACKING_LEN},
    {kg_djb2_hash("speak", 5), "speak", 5, KG_SPEAK_CLASSES, KG_SPEAK_LEN},
    {kg_djb2_hash("talking", 7), "talking", 7, KG_TALKING_CLASSES, KG_TALKING_LEN},
    {kg_djb2_hash("tea", 3), "tea", 3, KG_TEA_CLASSES, KG_TEA_LEN},
    {kg_djb2_hash("throw", 5), "throw", 5, KG_THROW_CLASSES, KG_THROW_LEN},
    {kg_djb2_hash("walk", 4), "walk", 4, KG_WALK_CLASSES, KG_WALK_LEN},
    {kg_djb2_hash("wear", 4), "wear", 4, KG_WEAR_CLASSES, KG_WEAR_LEN},
    {kg_djb2_hash("work", 4), "work", 4, KG_WORK_CLASSES, KG_WORK_LEN},
};

constexpr uint8_t KG_LOOKUP_TABLE_SIZE = 49;

// ============================================================================
// Branchless Lowercase (no locale, no stdlib tolower)
// ============================================================================

constexpr uint8_t kg_to_lower(uint8_t c)
{
    // If c is in ['A'..'Z'], subtract 32 to convert to lowercase
    // Otherwise, return unchanged
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

// ============================================================================
// Stop-word Check (minimal set for embedded)
// ============================================================================

constexpr bool kg_is_stop_word(const char *token, uint8_t len)
{
    // Hardcoded check for common stop words (no dynamic lookup)
    if (len == 1)
    {
        return token[0] == 'a';
    }
    if (len == 2)
    {
        return (token[0] == 't' && token[1] == 'o') ||
               (token[0] == 'i' && token[1] == 's');
    }
    if (len == 3)
    {
        return (token[0] == 't' && token[1] == 'h' && token[2] == 'e');
    }
    // Add more as needed; this is a minimal set for demo
    return false;
}

// ============================================================================
// Tokenizer (in-place, no allocation)
// ============================================================================

struct KgToken
{
    const char *ptr;
    uint8_t len;
};

constexpr bool kg_is_token_char(uint8_t c)
{
    // Token chars: a-z, 0-9, underscore, hyphen
    return (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '_' || c == '-';
}

uint8_t kg_tokenize(const char *prompt, uint16_t len, KgToken tokens[KG_MAX_TOKENS])
{
    uint8_t token_count = 0;
    uint16_t i = 0;

    while (i < len && token_count < KG_MAX_TOKENS)
    {
        // Skip whitespace and punctuation
        while (i < len && !kg_is_token_char(kg_to_lower((uint8_t)prompt[i])))
        {
            i++;
        }

        // If we found a token start
        if (i < len)
        {
            const char *token_start = prompt + i;
            uint8_t token_len = 0;

            // Collect token characters (lowercased on the fly, stored inline)
            while (i < len && kg_is_token_char(kg_to_lower((uint8_t)prompt[i])))
            {
                token_len++;
                i++;
            }

            // Skip stop words
            if (!kg_is_stop_word(token_start, token_len))
            {
                tokens[token_count].ptr = token_start;
                tokens[token_count].len = token_len;
                token_count++;
            }
        }
    }

    return token_count;
}

// ============================================================================
// String Match (case-insensitive, for collision detection)
// ============================================================================

bool kg_streq_lower(const char *a, uint8_t a_len, const char *b, uint8_t b_len)
{
    if (a_len != b_len)
        return false;
    for (uint8_t i = 0; i < a_len; ++i)
    {
        if (kg_to_lower((uint8_t)a[i]) != kg_to_lower((uint8_t)b[i]))
        {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Main Lookup Function
// ============================================================================

KgResult kg_lookup(const char *prompt, uint16_t len)
{
    // Step 1: Truncate
    if (len > KG_MAX_PROMPT_CHARS)
    {
        len = KG_MAX_PROMPT_CHARS;
    }

    // Step 2: Tokenize (stack-based, fixed size)
    KgToken tokens[KG_MAX_TOKENS];
    uint8_t token_count = kg_tokenize(prompt, len, tokens);

    // Step 3: Lookup first token (first match wins)
    for (uint8_t i = 0; i < token_count; ++i)
    {
        const KgToken &tok = tokens[i];

        // Compute hash for this token (lowercased) - MUST match kg_djb2_hash initial value
        uint32_t tok_hash = 5381;
        for (uint8_t j = 0; j < tok.len; ++j)
        {
            uint8_t c = kg_to_lower((uint8_t)tok.ptr[j]);
            tok_hash = ((tok_hash << 5) + tok_hash) ^ c;
        }

        // Linear search in lookup table (small table, fast)
        for (uint8_t j = 0; j < KG_LOOKUP_TABLE_SIZE; ++j)
        {
            const KgEntry &entry = KG_LOOKUP_TABLE[j];

            // Hash match
            if (entry.hash == tok_hash)
            {
                // Collision check: verify string match
                if (kg_streq_lower(tok.ptr, tok.len, entry.token, entry.token_len))
                {
                    // FOUND: return immediately
                    return KgResult{entry.classes, entry.count, true};
                }
            }
        }
    }

    // Step 4: No match
    return KgResult{nullptr, 0, false};
}
