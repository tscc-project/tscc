interface Item284 { id: number; name: string; }
type Key284 = string | number;
export function make284(id: number, name: string): Item284 {
  const value: Item284 = {id, name};
  return value as Item284;
}
export const item284: Item284 = make284(284, "item-284");
