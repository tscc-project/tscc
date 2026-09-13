interface Box { value:number }
const box:Box={value:1};
const selected:number=true ? box?.value! : 2;
let count:number=0; count++;
const ok:number=(selected + count) satisfies number;
if (selected >= 1 && count === 1) { count += 1; }
