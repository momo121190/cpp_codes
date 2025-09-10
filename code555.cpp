#include "QagtArchiver.h"
#include <QDebug>

// Instance unique static avec initialisation
static QSimpleCrypt s_crypto(0x0C2AD4A4ACB9F023);

void QagtArchiver::initialize(quint64 cryptoKey)
{
    s_crypto.setKey(cryptoKey);
    s_crypto.setCompressionMode(QSimpleCrypt::CompressionAlways);
    s_crypto.setIntegrityProtectionMode(QSimpleCrypt::ProtectionChecksum);
    qDebug() << "QagtArchiver initialisé avec clé:" << cryptoKey;
}

QSimpleCrypt& QagtArchiver::getCryptoInstance()
{
    // Initialisation lazy au premier appel
    static bool initialized = false;
    if (!initialized) {
        initialize(); // Utilise la clé par défaut
        initialized = true;
    }
    return s_crypto;
}

void QagtArchiver::encryptStream(QString &data)
{
    QSimpleCrypt& crypto = getCryptoInstance();
    data = crypto.encryptToString(data);
}

void QagtArchiver::decryptStream(QString &data)
{
    QSimpleCrypt& crypto = getCryptoInstance();
    data = crypto.decryptToString(data);
    
    // Vérification d'erreur optionnelle
    if (crypto.lastError() != QSimpleCrypt::ErrorNoError) {
        qWarning() << "Erreur déchiffrement SimpleCrypt:" << crypto.lastError();
    }
}

bool QagtArchiver::testSymmetry(const QString &testData)
{
    QString original = testData;
    QString encrypted = original;
    
    encryptStream(encrypted);
    decryptStream(encrypted);
    
    bool success = (encrypted == original);
    
    qDebug() << "Test symétrie SimpleCrypt:";
    qDebug() << "Original:  " << original;
    qDebug() << "Resultat:  " << encrypted;
    qDebug() << "Succès:    " << success;
    
    return success;
}
