namespace N63 {
  export const base: number = 63;
  export function add(x: number): number { return base + x; }
}
function over63(x: number): number;
function over63(x: string): string;
function over63(x: number | string): number | string { return x; }
abstract class A63 { abstract f(x: number): number; }
class B63 extends A63 { override f(x: number): number { return x + 1; } }
enum E63 { A = 5 / 2, B, C = 2 ** 3, D }
const angle63 = <number>(63 + 1);
const check63 = {x: angle63} satisfies {x: number};
console.log(N63.add(1), over63(63), new B63().f(1), E63.B, E63.D, check63.x);
