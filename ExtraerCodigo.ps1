# ====================================================================
# SCRIPT PARA RECOPILAR CÓDIGO FUENTE PARA ANÁLISIS DE IA
# ====================================================================
# 1. Configuración de la ruta principal (Modifícala si cambias de proyecto)
$BasePath = "C:\Users\valer\source\repos\OPThread"
# 2. Nombre del archivo de salida (Se guardará en la misma ruta principal)
$OutputFile = Join-Path -Path $BasePath -ChildPath "CodigoContextoIA.txt"
# 3. Carpetas a IGNORAR (No queremos compilar basura ni binarios)
$ExcludeFolders = @("out", "x64", "assets", "build", ".vs", ".git")
# 4. Extensiones de código a INCLUIR
$IncludeExtensions = @(".cpp", ".c", ".h", ".hpp")
# 5. Archivos específicos sin extensión típica a INCLUIR
$SpecificFiles = @("CMakeLists.txt")
# ====================================================================

Write-Host "Iniciando recopilación de código en: $BasePath" -ForegroundColor Cyan

# Eliminar el archivo de salida si ya existe de una ejecución anterior
if (Test-Path $OutputFile) { Remove-Item $OutputFile }

# Crear encabezado del documento
Add-Content -Path $OutputFile -Value "=== CONTEXTO DE PROYECTO PARA IA ==="
Add-Content -Path $OutputFile -Value "Ruta Base: $BasePath"
Add-Content -Path $OutputFile -Value "Fecha de generación: $(Get-Date)`n"

# Obtener todos los archivos recursivamente
$AllFiles = Get-ChildItem -Path $BasePath -Recurse -File
$FileCount = 0

foreach ($File in $AllFiles) {
    # Evitar leer el propio archivo de salida si se está guardando en la misma carpeta
    if ($File.FullName -eq $OutputFile) { continue }

    # Verificar si el archivo está dentro de una carpeta excluida
    $Skip = $false
    foreach ($Folder in $ExcludeFolders) {
        # Validar si el nombre de la carpeta excluida es parte de la ruta
        if ($File.DirectoryName -match "\\$Folder(\\|$)") {
            $Skip = $true
            break
        }
    }
    if ($Skip) { continue }

    # Verificar si es un archivo que queremos leer (por extensión o nombre específico)
    $IsMatch = $false
    if ($IncludeExtensions -contains $File.Extension) {
        $IsMatch = $true
    } elseif ($SpecificFiles -contains $File.Name) {
        $IsMatch = $true
    }

    if ($IsMatch) {
        # Obtener la ruta relativa (ej. "src\main.cpp" en lugar de toda la ruta larga)
        $RelativePath = $File.FullName.Substring($BasePath.Length + 1)
        
        # Escribir un separador claro para la IA con el nombre del archivo
        $Header = "`n" + ("=" * 60) + "`n" + "ARCHIVO: $RelativePath" + "`n" + ("=" * 60)
        Add-Content -Path $OutputFile -Value $Header
        
        # Leer el contenido del archivo de código y agregarlo al txt
        $Content = Get-Content -Path $File.FullName -Raw -ErrorAction SilentlyContinue
        if ($Content) {
            Add-Content -Path $OutputFile -Value $Content
        } else {
            Add-Content -Path $OutputFile -Value "// [Archivo vacío]"
        }
        
        $FileCount++
        Write-Host "Procesado: $RelativePath" -ForegroundColor DarkGray
    }
}

# --- NUEVO: Copiar el contenido al portapapeles ---
Get-Content -Path $OutputFile -Raw | Set-Clipboard

Write-Host "`n¡Proceso completado con éxito!" -ForegroundColor Green
Write-Host "Se procesaron $FileCount archivos." -ForegroundColor Green
Write-Host "El archivo para la IA está listo en: $OutputFile" -ForegroundColor Yellow
Write-Host "¡Todo el código se ha copiado automáticamente al portapapeles!" -ForegroundColor Cyan