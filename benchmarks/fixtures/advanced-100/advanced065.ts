namespace N65 {
  export const base: number = 65;
  export function add(x: number): number { return base + x; }
}
function over65(x: number): number;
function over65(x: string): string;
function over65(x: number | string): number | string { return x; }
abstract class A65 { abstract f(x: number): number; }
class B65 extends A65 { override f(x: number): number { return x + 1; } }
enum E65 { A = 5 / 2, B, C = 2 ** 3, D }
const angle65 = <number>(65 + 1);
const check65 = {x: angle65} satisfies {x: number};
console.log(N65.add(1), over65(65), new B65().f(1), E65.B, E65.D, check65.x);
