namespace N72 {
  export const base: number = 72;
  export function add(x: number): number { return base + x; }
}
function over72(x: number): number;
function over72(x: string): string;
function over72(x: number | string): number | string { return x; }
abstract class A72 { abstract f(x: number): number; }
class B72 extends A72 { override f(x: number): number { return x + 1; } }
enum E72 { A = 5 / 2, B, C = 2 ** 3, D }
const angle72 = <number>(72 + 1);
const check72 = {x: angle72} satisfies {x: number};
console.log(N72.add(1), over72(72), new B72().f(1), E72.B, E72.D, check72.x);
