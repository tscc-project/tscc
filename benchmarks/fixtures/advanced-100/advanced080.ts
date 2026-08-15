namespace N80 {
  export const base: number = 80;
  export function add(x: number): number { return base + x; }
}
function over80(x: number): number;
function over80(x: string): string;
function over80(x: number | string): number | string { return x; }
abstract class A80 { abstract f(x: number): number; }
class B80 extends A80 { override f(x: number): number { return x + 1; } }
enum E80 { A = 5 / 2, B, C = 2 ** 3, D }
const angle80 = <number>(80 + 1);
const check80 = {x: angle80} satisfies {x: number};
console.log(N80.add(1), over80(80), new B80().f(1), E80.B, E80.D, check80.x);
