interface Item278 { id: number; name: string; }
type Key278 = string | number;
export function make278(id: number, name: string): Item278 {
  const value: Item278 = {id, name};
  return value as Item278;
}
export const item278: Item278 = make278(278, "item-278");
