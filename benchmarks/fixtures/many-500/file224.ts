interface Item224 { id: number; name: string; }
type Key224 = string | number;
export function make224(id: number, name: string): Item224 {
  const value: Item224 = {id, name};
  return value as Item224;
}
export const item224: Item224 = make224(224, "item-224");
