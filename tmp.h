4AEUczZJsvwQ8iqtdZ6XJJMgI9yHCM4M44AyesbB3rIiGQD2f4+9KdPW9e0=

WE2wdxaZM9fJ9jUFYtucRYzhBdyoz3P4usA500aii0n0J9k7
M/qjlaxOYJOkhYoJa8zh7JW3oXyJJ5YSjF5XkmF1wUFEofCb0W9M+RmiABk=

  import base64
import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

class UrlEncryption:
    def __init__(self):
        self.secretKey = None
        self.encoding_type = "UTF-8"
        self.gcm_iv_length = 12
        self.gcm_tag_length = 16

    def set_key(self, myKey):
        backend = default_backend()
        salt = b''  # Using an empty salt for compatibility with the Java code
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA1(),
            length=16,
            salt=salt,
            iterations=1,  # Adjust iterations as needed for security
            backend=backend
        )
        key = kdf.derive(myKey.encode(self.encoding_type))
        self.secretKey = key

    def encrypt(self, strToEncrypt, secret):
        try:
            self.set_key(secret)
            iv = os.urandom(self.gcm_iv_length)
            cipher = Cipher(algorithms.AES(self.secretKey), modes.GCM(iv), backend=default_backend())
            encryptor = cipher.encryptor()
            padder = padding.PKCS7(algorithms.AES.block_size).padder()
            padded_data = padder.update(strToEncrypt.encode(self.encoding_type)) + padder.finalize()
            ciphertext = encryptor.update(padded_data) + encryptor.finalize()
            encrypted_data = iv + ciphertext + encryptor.tag
            encoded = base64.b64encode(encrypted_data).decode(self.encoding_type)
            return encoded

        except Exception as e:
            print(e)
            return None

# Example usage:
urlEncryption = UrlEncryption()
sha1 = "ENCRYPTSITIUSERID"
encrypted_text = urlEncryption.encrypt("ACCBATCH", sha1)
print(encrypted_text)
