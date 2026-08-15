interface Item63 { id: number; name: string; }
type Key63 = string | number;
export function make63(id: number, name: string): Item63 {
  const value: Item63 = {id, name};
  return value as Item63;
}
export const item63: Item63 = make63(63, "item-63");
