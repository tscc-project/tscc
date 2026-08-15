interface Item203 { id: number; name: string; }
type Key203 = string | number;
export function make203(id: number, name: string): Item203 {
  const value: Item203 = {id, name};
  return value as Item203;
}
export const item203: Item203 = make203(203, "item-203");
