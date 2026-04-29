"""
app.py - Flask web uygulaması
C kodunu derler ve web isteklerine bağlar
"""

from flask import Flask, render_template, request, jsonify
import subprocess
import json
import os
import sys

app = Flask(__name__)

# C programının yolu
C_SOURCE = "resistor_logic.c"
C_BINARY = "resistor_logic"

def compile_c_program():
    #C kodunu derler
    if os.path.exists(C_BINARY):
        return True
    
    try:
        result = subprocess.run(
            ["gcc", C_SOURCE, "-o", C_BINARY, "-lm"],
            capture_output=True,
            text=True
        )
        if result.returncode != 0:
            print(f"Derleme hatası: {result.stderr}")
            return False
        return True
    except FileNotFoundError:
        print("HATA: gcc bulunamadı! Lütfen GCC'yi kurun.")
        return False

@app.route('/')
def index():
    """Ana sayfa - direnç hesaplayıcı arayüzü"""
    return render_template('index.html')

@app.route('/calculate', methods=['POST'])
def calculate():
    # Renkleri alır, C programını çalıştırır, JSON sonuç döndürür
    
    data = request.get_json()
    bands = data.get('bands', [])
    
    if not bands:
        return jsonify({"hata": "Renk seçilmedi"}), 400
    
    # C programını çağır
    try:
        result = subprocess.run(
            [f"./{C_BINARY}"] + bands,
            capture_output=True,
            text=True,
            timeout=5
        )
        
        # Çıktıyı parse et
        output = result.stdout.strip()
        
        if result.returncode != 0:
            try:
                error_json = json.loads(output)
                return jsonify(error_json), 400
            except:
                return jsonify({"hata": output or "Hesaplama hatası"}), 400
        
        # JSON parse et ve döndür
        try:
            return jsonify(json.loads(output))
        except json.JSONDecodeError:
            return jsonify({"hata": f"Beklenmeyen çıktı: {output}"}), 500
            
    except subprocess.TimeoutExpired:
        return jsonify({"hata": "Hesaplama zaman aşımına uğradı"}), 500
    except Exception as e:
        return jsonify({"hata": f"Sistem hatası: {str(e)}"}), 500

if __name__ == '__main__':
    print("🔧 C programı derleniyor...")
    if not compile_c_program():
        print("❌ Derleme başarısız! Çıkılıyor.")
        sys.exit(1)
    print("✅ Derleme başarılı!")
    print("🌐 Sunucu başlatılıyor: http://localhost:5000")
    app.run(debug=True)


    