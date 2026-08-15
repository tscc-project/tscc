interface Item357 { id: number; name: string; }
type Key357 = string | number;
export function make357(id: number, name: string): Item357 {
  const value: Item357 = {id, name};
  return value as Item357;
}
export const item357: Item357 = make357(357, "item-357");
