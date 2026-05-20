#include "firebase_helper.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

FirebaseData   fbdo;
FirebaseAuth   auth;
FirebaseConfig firebaseConfig;
bool           signupOK = false;

// ─── Internal logging helper ──────────────────────────────────────────────────
static void logResult(bool ok, const char *tag) {
  if (ok) {
    Serial.print("[Firebase] OK  – ");
    Serial.println(tag);
  } else {
    Serial.print("[Firebase] ERR – ");
    Serial.print(tag);
    Serial.print(" : ");
    Serial.println(fbdo.errorReason());
  }
}

// ─── Public API ───────────────────────────────────────────────────────────────
void firebaseInit() {
  firebaseConfig.api_key      = API_KEY;
  firebaseConfig.database_url = DATABASE_URL;

  if (Firebase.signUp(&firebaseConfig, &auth, "", "")) {
    Serial.println("[Firebase] Anonymous sign-up OK");
    signupOK = true;
  } else {
    Serial.printf("[Firebase] Sign-up failed: %s\n",
                  firebaseConfig.signer.signupError.message.c_str());
  }

  firebaseConfig.token_status_callback = tokenStatusCallback;
  Firebase.begin(&firebaseConfig, &auth);
  Firebase.reconnectWiFi(true);
}

bool firebaseSetString(const char *path, const char *field, const String &value) {
  FirebaseJson json;
  json.set(field, value);
  bool ok = Firebase.RTDB.setJSON(&fbdo, path, &json);
  logResult(ok, path);
  return ok;
}

bool firebaseSetBool(const char *path, const char *field, bool value) {
  FirebaseJson json;
  json.set(field, value);
  bool ok = Firebase.RTDB.setJSON(&fbdo, path, &json);
  logResult(ok, path);
  return ok;
}

bool firebaseGetBool(const char *path) {
  if (Firebase.RTDB.getBool(&fbdo, path)) {
    return fbdo.boolData();
  }
  Serial.print("[Firebase] getBool failed: ");
  Serial.println(fbdo.errorReason());
  return false;
}
