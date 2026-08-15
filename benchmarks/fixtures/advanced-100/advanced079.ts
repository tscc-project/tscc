namespace N79 {
  export const base: number = 79;
  export function add(x: number): number { return base + x; }
}
function over79(x: number): number;
function over79(x: string): string;
function over79(x: number | string): number | string { return x; }
abstract class A79 { abstract f(x: number): number; }
class B79 extends A79 { override f(x: number): number { return x + 1; } }
enum E79 { A = 5 / 2, B, C = 2 ** 3, D }
const angle79 = <number>(79 + 1);
const check79 = {x: angle79} satisfies {x: number};
console.log(N79.add(1), over79(79), new B79().f(1), E79.B, E79.D, check79.x);
