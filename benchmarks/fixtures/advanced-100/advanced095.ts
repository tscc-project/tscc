namespace N95 {
  export const base: number = 95;
  export function add(x: number): number { return base + x; }
}
function over95(x: number): number;
function over95(x: string): string;
function over95(x: number | string): number | string { return x; }
abstract class A95 { abstract f(x: number): number; }
class B95 extends A95 { override f(x: number): number { return x + 1; } }
enum E95 { A = 5 / 2, B, C = 2 ** 3, D }
const angle95 = <number>(95 + 1);
const check95 = {x: angle95} satisfies {x: number};
console.log(N95.add(1), over95(95), new B95().f(1), E95.B, E95.D, check95.x);
