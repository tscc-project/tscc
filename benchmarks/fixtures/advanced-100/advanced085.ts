namespace N85 {
  export const base: number = 85;
  export function add(x: number): number { return base + x; }
}
function over85(x: number): number;
function over85(x: string): string;
function over85(x: number | string): number | string { return x; }
abstract class A85 { abstract f(x: number): number; }
class B85 extends A85 { override f(x: number): number { return x + 1; } }
enum E85 { A = 5 / 2, B, C = 2 ** 3, D }
const angle85 = <number>(85 + 1);
const check85 = {x: angle85} satisfies {x: number};
console.log(N85.add(1), over85(85), new B85().f(1), E85.B, E85.D, check85.x);
