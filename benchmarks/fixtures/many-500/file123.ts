interface Item123 { id: number; name: string; }
type Key123 = string | number;
export function make123(id: number, name: string): Item123 {
  const value: Item123 = {id, name};
  return value as Item123;
}
export const item123: Item123 = make123(123, "item-123");
