/* Offline learning navigation. No requests, external services or code execution. */
(() => {
  'use strict';
  const book=JSON.parse(document.getElementById('book-data').textContent);
  const main=document.getElementById('main'), nav=document.getElementById('nav');
  const storageKey='ct2-study-progress-v1'; let progress={}, timer=null;
  try { progress=JSON.parse(localStorage.getItem(storageKey)||'{}'); } catch (_) { progress={}; }
  const docs=new Map(book.docs.map(d=>[d.id,d]));
  const questions=new Map([...book.exercises,...book.scenarios].map(q=>[q.id,q]));
  const sourceMap=new Map(book.sources.map(s=>[s.path,s]));
  const el=(tag,text,cls)=>{const n=document.createElement(tag);if(text!==undefined)n.textContent=text;if(cls)n.className=cls;return n;};
  function markdown(text,parent=main) { const n=el('section'); n.innerHTML=marked.parse(text); parent.append(n);return n; }
  function details(title,text,parent=main) { const d=el('details');d.append(el('summary',title));markdown(text,d);parent.append(d);return d; }
  function save() {try{localStorage.setItem(storageKey,JSON.stringify(progress));}catch(_){const note=el('p','브라우저에서 진도 저장을 허용하지 않습니다. 자료 열람은 가능합니다.','muted');main.append(note);}}
  function progressPanel(id) {
    const panel=el('div',undefined,'progress');
    for(const [key,title] of [['read','읽음'],['code','직접 작성'],['run','사용자 실행 확인']]) {
      const label=el('label'),input=el('input');input.type='checkbox';input.checked=Boolean(progress[id]?.[key]);
      input.addEventListener('change',()=>{progress[id]??={};progress[id][key]=input.checked;save();});
      label.append(input,document.createTextNode(title));panel.append(label);
    } main.append(panel);
  }
  function timerPanel(minutes) {
    const panel=el('div',undefined,'timer'),label=el('span'),start=el('button','연습 시작'),reset=el('button','초기화');
    let remaining=minutes*60,end=0,running=false;
    const show=()=>{label.textContent=`권장 연습 ${Math.floor(remaining/60)}:${String(remaining%60).padStart(2,'0')} · 실제 시험 시간 아님`;};
    const stop=()=>{clearInterval(timer);timer=null;running=false;start.textContent='계속';};
    start.onclick=()=>{if(running){remaining=Math.max(0,Math.ceil((end-Date.now())/1000));stop();show();return;}
      end=Date.now()+remaining*1000;running=true;start.textContent='정지';
      timer=setInterval(()=>{remaining=Math.max(0,Math.ceil((end-Date.now())/1000));show();if(!remaining){stop();label.textContent+=' · 시간 종료: 미완료 조건을 기록하세요.';}},250);
    };
    reset.onclick=()=>{stop();remaining=minutes*60;start.textContent='연습 시작';show();};
    show();panel.append(label,start,reset);main.append(panel);
  }
  function renderQuestion(q) {
    main.append(el('p',`${q.id} / ${q.track==='native'?'순정 OpenGL':'Standard'}`,'eyebrow'),el('h1',q.title));
    progressPanel(q.id);timerPanel(q.minutes);
    markdown('## 문제\n\n'+q.problem);
    details('상태·공간 설계 보기',q.design);
    q.hints.forEach((h,i)=>details(`힌트 ${i+1}`,h));
    details('해설과 수정 코드 펼치기',q.solution);
    markdown('## 사용자 관찰 기준\n\n'+q.observe.map(o=>'- '+o).join('\n'));
    const refs=el('section');refs.append(el('h2','코드·학습 참조'));
    for(const ref of q.refs) {
      const link=el('a',ref);const sourceRef=ref.split(':')[0];const source=sourceMap.has(sourceRef)?sourceRef:sourceMap.has('coding-test-2/'+sourceRef)?'coding-test-2/'+sourceRef:null;
      const id=ref.split(':')[0];
      if(source)link.href='#source:'+source;
      else if(docs.has(id)||questions.has(id))link.href='#'+id;
      else {link.href=ref.startsWith('standard/')||ref.startsWith('framework/')?'../'+ref:ref;}
      const row=el('p');row.append(link);refs.append(row);
    } main.append(refs);
    const area=el('textarea');area.rows=5;area.placeholder='처음 실패한 지점 / 상태·좌표계·uniform의 원인 / 다음에 바꿀 한 가지';area.setAttribute('aria-label','문항 복기 메모');area.value=progress[q.id]?.note||'';
    area.onchange=()=>{progress[q.id]??={};progress[q.id].note=area.value;save();};main.append(el('h2','직접 복기'),area);
  }
  function renderSource(path) {
    const source=sourceMap.get(path);if(!source){main.append(el('p','소스 스냅샷이 없습니다.'));return;}
    main.append(el('h1',path),el('p','생성 시점의 전체 소스입니다. 실제 편집 뒤 행 번호가 달라질 수 있습니다. 코드를 실행하지 않습니다.','muted'));
    const p=el('p');const link=el('a','실제 원본 파일 열기');link.href='../'+path;p.append(link);main.append(p);
    if(source.hooks.length) markdown('## 위치 표식\n\n'+source.hooks.map(h=>'- `'+h+'`').join('\n'));
    const wrap=el('div',undefined,'source-wrap'),button=el('button','전체 코드 복사'),list=el('ol',undefined,'source');
    button.onclick=()=>copy(source.text,button);
    source.text.trimEnd().split(/\r?\n/).forEach(line=>{const li=el('li'),code=el('code',line||' ');li.append(code);list.append(li);});
    wrap.append(button,list);main.append(wrap);
  }
  async function copy(text,button) {
    let ok=false;
    try{if(navigator.clipboard){await navigator.clipboard.writeText(text);ok=true;}}catch(_){}
    if(!ok){const t=el('textarea');t.value=text;t.style.position='fixed';t.style.opacity='0';document.body.append(t);t.select();try{ok=document.execCommand('copy');}catch(_){}t.remove();}
    button.textContent=ok?'복사됨':'직접 선택해 복사하세요';
  }
  function codeButtons() {main.querySelectorAll('pre').forEach(pre=>{const b=el('button','코드 복사');b.className='copy';b.onclick=()=>copy(pre.querySelector('code')?.textContent||pre.textContent,b);pre.prepend(b);});}
  function render() {
    if(timer){clearInterval(timer);timer=null;}main.replaceChildren();
    let id;try{id=decodeURIComponent(location.hash.slice(1))||'README.md';}catch(_){id='README.md';}
    if(questions.has(id))renderQuestion(questions.get(id));
    else if(id.startsWith('source:'))renderSource(id.slice(7));
    else if(id==='sources'){main.append(el('h1','전체 소스 읽기'));book.sources.forEach(s=>{const a=el('a',s.path);a.href='#source:'+s.path;const row=el('p');row.append(a);main.append(row);});}
    else {const d=docs.get(id)||docs.get('README.md');markdown(d.markdown);progressPanel(d.id);}
    codeButtons();nav.querySelectorAll('a').forEach(a=>a.classList.toggle('active',a.hash==='#'+id));window.scrollTo(0,0);
  }
  function navigation(filter='') {
    nav.replaceChildren();
    for(const [title,entries] of [['학습과 구현',book.docs],['순정 N제',book.exercises.filter(q=>q.track==='native')],['Standard N제',book.exercises.filter(q=>q.track==='standard')],['통합 시나리오',book.scenarios]]) {
      const group=el('div');group.append(el('h2',title));let count=0;
      entries.forEach(x=>{if(!(x.id+' '+x.title).toLowerCase().includes(filter.toLowerCase()))return;const a=el('a',(x.id.startsWith('CT2')?x.id+' · ':'')+x.title);a.href='#'+x.id;group.append(a);count++;});if(count)nav.append(group);
    } const a=el('a','전체 코드 · 실제 행 번호');a.href='#sources';nav.append(a);
  }
  main.addEventListener('click',event=>{
    const a=event.target.closest('a');if(!a)return;
    const href=a.getAttribute('href');if(!href||href.startsWith('#')||/^[a-z]+:/i.test(href))return;
    const current=decodeURIComponent(location.hash.slice(1))||'README.md';
    const base=current.includes('/')?current.slice(0,current.lastIndexOf('/')+1):'';
    const normalized=new URL(base+href,'https://offline.invalid/').pathname.slice(1);
    if(docs.has(normalized)||questions.has(normalized)){event.preventDefault();location.hash=normalized;}
    else if(/CT2-[NS]\d\d\.md$/.test(normalized)){const id=normalized.match(/CT2-[NS]\d\d/)[0];if(questions.has(id)){event.preventDefault();location.hash=id;}}
  });
  const now=new Date(),today=Date.UTC(now.getFullYear(),now.getMonth(),now.getDate()),exam=Date.UTC(2026,10,24),days=Math.round((exam-today)/86400000);
  document.getElementById('deadline').textContent=days>0?`D-${days}`:days===0?'시험일':`시험일 +${-days}일`;
  document.getElementById('search').oninput=e=>navigation(e.target.value);
  window.addEventListener('hashchange',render);navigation();render();
})();