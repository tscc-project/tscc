namespace N64 {
  export const base: number = 64;
  export function add(x: number): number { return base + x; }
}
function over64(x: number): number;
function over64(x: string): string;
function over64(x: number | string): number | string { return x; }
abstract class A64 { abstract f(x: number): number; }
class B64 extends A64 { override f(x: number): number { return x + 1; } }
enum E64 { A = 5 / 2, B, C = 2 ** 3, D }
const angle64 = <number>(64 + 1);
const check64 = {x: angle64} satisfies {x: number};
console.log(N64.add(1), over64(64), new B64().f(1), E64.B, E64.D, check64.x);
