interface Item241 { id: number; name: string; }
type Key241 = string | number;
export function make241(id: number, name: string): Item241 {
  const value: Item241 = {id, name};
  return value as Item241;
}
export const item241: Item241 = make241(241, "item-241");
