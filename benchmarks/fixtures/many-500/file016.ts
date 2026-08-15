interface Item16 { id: number; name: string; }
type Key16 = string | number;
export function make16(id: number, name: string): Item16 {
  const value: Item16 = {id, name};
  return value as Item16;
}
export const item16: Item16 = make16(16, "item-16");
