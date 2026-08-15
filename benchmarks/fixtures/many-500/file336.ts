interface Item336 { id: number; name: string; }
type Key336 = string | number;
export function make336(id: number, name: string): Item336 {
  const value: Item336 = {id, name};
  return value as Item336;
}
export const item336: Item336 = make336(336, "item-336");
