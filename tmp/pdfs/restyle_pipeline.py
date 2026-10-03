from pathlib import Path
p=Path('tmp/pdfs/build_pipeline.py')
s=p.read_text(encoding='utf-8-sig')
s=s.replace('import math','from reportlab.pdfbase import pdfmetrics\nfrom reportlab.pdfbase.ttfonts import TTFont\npdfmetrics.registerFont(TTFont("Segoe", "C:/Windows/Fonts/segoeui.ttf"))\npdfmetrics.registerFont(TTFont("SegoeBold", "C:/Windows/Fonts/segoeuib.ttf"))\nimport math')
s=s.replace('pipeline-vectorial.pdf','pipeline-memoria.pdf').replace("'Helvetica-Bold' if bold else 'Helvetica'","'SegoeBold' if bold else 'Segoe'")
s=s.replace("'#101820'","'#29343d'").replace("'#18232b'","'#45525d'").replace("fill='#e5eff8'","fill='#fafcfd'")
s=s.replace("c.setLineWidth(1.8)","c.setLineWidth(1.5)")
s=s.replace("def poly(pts,fill='#fafcfd'):\n", """def poly(pts,fill='#fafcfd'):
 if fill != '#45525d':
  c.saveState()
  for offset,alpha in [(4,0.025),(2.5,0.04),(1.2,0.05)]:
   c.setFillColor(col('#43505b'));c.setFillAlpha(alpha)
   q=c.beginPath();q.moveTo(pts[0][0]+offset,H-pts[0][1]-offset)
   for x,y in pts[1:]: q.lineTo(x+offset,H-y-offset)
   q.close();c.drawPath(q,fill=1,stroke=0)
  c.restoreState()
""")
a=s.index('def panel('); b=s.index("panel(10",a)
s=s[:a]+'''def panel(x,w,title,sub,bg,head,edge):
 c.saveState();c.setFillColor(col('#35404a'));c.setFillAlpha(0.055)
 c.roundRect(x+2,H-518,w,500,10,fill=1,stroke=0);c.restoreState()
 c.setFillColor(col(bg));c.setStrokeColor(col(edge));c.setLineWidth(1.2);c.roundRect(x,H-515,w,500,10,fill=1,stroke=1)
 c.saveState()
 clip=c.beginPath();clip.roundRect(x+0.7,H-514.3,w-1.4,498.6,9.3);c.clipPath(clip,stroke=0)
 c.setFillColor(col(head));c.rect(x,H-110,w,95,fill=1,stroke=0);c.restoreState()
 c.setStrokeColor(col(edge));c.setLineWidth(0.6);c.line(x+1,H-110,x+w-1,H-110)
 text(x+w/2,58,title,35,True);text(x+w/2,91,sub,24,False)
''' +s[b:]
for old,new in {'#fff0f0':'#f8f3f2','#ffdddd':'#eaddda','#ed5454':'#b9a19a','#fff9e7':'#f8f6ef','#fff0bc':'#ece6d5','#e9b527':'#b8ad8f','#edfaef':'#f1f6f2','#d1f3d7':'#dde8df','#45b764':'#9caf9f','#edf6ff':'#f1f5f8','#d3e9ff':'#dce6ed','#448ed3':'#9daebc','#f6efff':'#f5f2f7','#e7d8fa':'#e5dfec','#a36bd1':'#ada0bc','#f5f1fa':'#fafcfd'}.items(): s=s.replace(old,new)
s=s.replace("n=len(s.split('\\n')); text(x+w/2,y+h/2-(n-1)*13+7,s)","n=len(s.split('\\n')); size=min(21,(w-14)/max(pdfmetrics.stringWidth(t,'Segoe',1) for t in s.split('\\n'))); text(x+w/2,y+h/2-(n-1)*size*0.625+size*0.33,s,size)")
Path('tmp/pdfs/build_pipeline_memoria.py').write_text(s,encoding='utf-8')
