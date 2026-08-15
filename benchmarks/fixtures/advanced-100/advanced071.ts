namespace N71 {
  export const base: number = 71;
  export function add(x: number): number { return base + x; }
}
function over71(x: number): number;
function over71(x: string): string;
function over71(x: number | string): number | string { return x; }
abstract class A71 { abstract f(x: number): number; }
class B71 extends A71 { override f(x: number): number { return x + 1; } }
enum E71 { A = 5 / 2, B, C = 2 ** 3, D }
const angle71 = <number>(71 + 1);
const check71 = {x: angle71} satisfies {x: number};
console.log(N71.add(1), over71(71), new B71().f(1), E71.B, E71.D, check71.x);
