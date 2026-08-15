interface Item313 { id: number; name: string; }
type Key313 = string | number;
export function make313(id: number, name: string): Item313 {
  const value: Item313 = {id, name};
  return value as Item313;
}
export const item313: Item313 = make313(313, "item-313");
