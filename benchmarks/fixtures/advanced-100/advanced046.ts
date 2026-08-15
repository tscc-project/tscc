namespace N46 {
  export const base: number = 46;
  export function add(x: number): number { return base + x; }
}
function over46(x: number): number;
function over46(x: string): string;
function over46(x: number | string): number | string { return x; }
abstract class A46 { abstract f(x: number): number; }
class B46 extends A46 { override f(x: number): number { return x + 1; } }
enum E46 { A = 5 / 2, B, C = 2 ** 3, D }
const angle46 = <number>(46 + 1);
const check46 = {x: angle46} satisfies {x: number};
console.log(N46.add(1), over46(46), new B46().f(1), E46.B, E46.D, check46.x);
