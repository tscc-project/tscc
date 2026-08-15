interface Item395 { id: number; name: string; }
type Key395 = string | number;
export function make395(id: number, name: string): Item395 {
  const value: Item395 = {id, name};
  return value as Item395;
}
export const item395: Item395 = make395(395, "item-395");
