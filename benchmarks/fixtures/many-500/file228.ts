interface Item228 { id: number; name: string; }
type Key228 = string | number;
export function make228(id: number, name: string): Item228 {
  const value: Item228 = {id, name};
  return value as Item228;
}
export const item228: Item228 = make228(228, "item-228");
