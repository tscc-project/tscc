namespace N83 {
  export const base: number = 83;
  export function add(x: number): number { return base + x; }
}
function over83(x: number): number;
function over83(x: string): string;
function over83(x: number | string): number | string { return x; }
abstract class A83 { abstract f(x: number): number; }
class B83 extends A83 { override f(x: number): number { return x + 1; } }
enum E83 { A = 5 / 2, B, C = 2 ** 3, D }
const angle83 = <number>(83 + 1);
const check83 = {x: angle83} satisfies {x: number};
console.log(N83.add(1), over83(83), new B83().f(1), E83.B, E83.D, check83.x);
