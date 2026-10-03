from pathlib import Path
p=Path('tmp/pdfs/build_pipeline_legible.py')
s=p.read_text(encoding='utf-8').replace(' bold=True',' bold=True\n if s == "Memoria\\nde\\ninstrucciones": size=21\n if s == "Instrucción" and x>450 and x<550: size=21')
s=s.replace("text(1694,244,'Dato leído',19)","text(1694,278,'Dato leído',19)")
p.write_text(s,encoding='utf-8')
