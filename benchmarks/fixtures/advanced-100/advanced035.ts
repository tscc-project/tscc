namespace N35 {
  export const base: number = 35;
  export function add(x: number): number { return base + x; }
}
function over35(x: number): number;
function over35(x: string): string;
function over35(x: number | string): number | string { return x; }
abstract class A35 { abstract f(x: number): number; }
class B35 extends A35 { override f(x: number): number { return x + 1; } }
enum E35 { A = 5 / 2, B, C = 2 ** 3, D }
const angle35 = <number>(35 + 1);
const check35 = {x: angle35} satisfies {x: number};
console.log(N35.add(1), over35(35), new B35().f(1), E35.B, E35.D, check35.x);
