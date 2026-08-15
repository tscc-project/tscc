interface Item335 { id: number; name: string; }
type Key335 = string | number;
export function make335(id: number, name: string): Item335 {
  const value: Item335 = {id, name};
  return value as Item335;
}
export const item335: Item335 = make335(335, "item-335");
