interface Item113 { id: number; name: string; }
type Key113 = string | number;
export function make113(id: number, name: string): Item113 {
  const value: Item113 = {id, name};
  return value as Item113;
}
export const item113: Item113 = make113(113, "item-113");
