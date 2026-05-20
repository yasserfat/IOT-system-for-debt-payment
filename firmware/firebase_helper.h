#pragma once

#include <Firebase_ESP_Client.h>
#include "config.h"

// ─── Global Firebase objects (defined in firebase_helper.cpp) ────────────────
extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig firebaseConfig;
extern bool signupOK;

// Initialise Firebase (anonymous sign-up + begin).
void firebaseInit();

// ─── Write helpers ────────────────────────────────────────────────────────────

// Write a string field to path as  { field: value }.
bool firebaseSetString(const char *path, const char *field, const String &value);

// Write a boolean field to path as  { field: value }.
bool firebaseSetBool(const char *path, const char *field, bool value);

// ─── Read helpers ─────────────────────────────────────────────────────────────

// Read a boolean from path; returns false on error.
bool firebaseGetBool(const char *path);
