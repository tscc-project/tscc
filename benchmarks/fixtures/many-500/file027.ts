interface Item27 { id: number; name: string; }
type Key27 = string | number;
export function make27(id: number, name: string): Item27 {
  const value: Item27 = {id, name};
  return value as Item27;
}
export const item27: Item27 = make27(27, "item-27");
